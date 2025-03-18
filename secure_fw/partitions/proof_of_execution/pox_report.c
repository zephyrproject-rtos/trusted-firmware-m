#include "pox_token.h"
#include "psa/crypto.h"
#include "tfm_sp_log.h" // TF-M Secure Partition Logging
#include "psa/initial_attestation.h"

#define SIGNATURE_BUFFER_SIZE 64

// Structure to store formatting options
struct sf_hex_tbl_fmt
{
    bool ascii;      // Flag to include ASCII representation
    bool addr_label; // Flag to show address labels
    uint32_t addr;   // Starting address
};

void print_hex(struct sf_hex_tbl_fmt *fmt, unsigned char *data, size_t len)
{
    uint32_t idx = 0;
    uint32_t cpos = fmt->addr % 16; // Current position in the row
    uint32_t ca = fmt->addr;        // Current address
    uint32_t ea = fmt->addr + len;  // End address

    if (!len)
    {
        return;
    }

    if (fmt->addr_label)
    {
        LOG_INFFMT("\n");
        LOG_INFFMT("          0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
        LOG_INFFMT("%08X ", fmt->addr - (fmt->addr % 16));
    }

    if (cpos != 0)
    {
        for (uint32_t i = 0; i < cpos; i++)
        {
            LOG_INFFMT("   ");
        }
    }

    while (len)
    {
        LOG_INFFMT("%X ", data[idx++]);
        cpos++;
        ca++;

        if (cpos == 16 || ca == ea)
        {
            LOG_INFFMT("\n");
            if (ca != ea && fmt->addr_label)
            {
                LOG_INFFMT("%X ", ca);
            }
            cpos = 0;
        }
        len--;
    }
    LOG_INFFMT("\n");
}

psa_status_t att_get_iat(uint8_t *challenge, uint8_t *token_buf, size_t *sys_token_sz)
{
    size_t token_buf_size = ATT_MAX_TOKEN_SIZE;
    LOG_INFFMT("[Secure] Requesting attestation token...\n");

    psa_status_t status = psa_initial_attest_get_token(challenge, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32, token_buf, token_buf_size, sys_token_sz);
    if (status != PSA_SUCCESS)
    {
        LOG_INFFMT("[Secure] ERROR: Failed to get attestation token (status: %d)\n", status);
        return status;
    }

    LOG_INFFMT("[Secure] Attestation token size: %d\n", *sys_token_sz);
    struct sf_hex_tbl_fmt fmt = {.ascii = false, .addr_label = false, .addr = 0};
    print_hex(&fmt, token_buf, token_buf_size);
    return PSA_SUCCESS;
}

psa_status_t generate_pox_report(uint8_t *token_buf, size_t token_size, uintptr_t faddr, int execution_output, uint8_t *cbor_report, size_t *cbor_report_len)
{
    if (!token_buf || !cbor_report || !cbor_report_len)
    {
        LOG_INFFMT("[Secure] ERROR: Null pointer argument in generate_pox_report.\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    LOG_INFFMT("[Secure] Initialize CBOR\n");
    UsefulBuf buffer = {cbor_report, cbor_report_len};
    QCBOREncodeContext encode_ctx;
    QCBOREncode_Init(&encode_ctx, buffer);

    QCBOREncode_OpenMap(&encode_ctx);
    QCBOREncode_AddBytesToMap(&encode_ctx, "ia_report", (UsefulBufC){token_buf, token_size});
    QCBOREncode_AddUInt64ToMap(&encode_ctx, "faddr", (uint64_t)faddr);
    QCBOREncode_AddInt64ToMap(&encode_ctx, "execution_output", (int64_t)execution_output);
    QCBOREncode_CloseMap(&encode_ctx);

    UsefulBufC encoded;
    QCBORError err = QCBOREncode_Finish(&encode_ctx, &encoded);
    if (err != QCBOR_SUCCESS)
    {
        LOG_INFFMT("[Secure] ERROR: Failed to encode CBOR report. QCBOR Error: %d\n", err);
        return PSA_ERROR_GENERIC_ERROR;
    }

    *cbor_report_len = encoded.len;

    uint8_t hash[32];
    size_t hash_len;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, encoded.ptr, encoded.len, hash, sizeof(hash), &hash_len);
    if (status != PSA_SUCCESS)
    {
        LOG_INFFMT("[Secure] ERROR: Failed to compute hash.\n");
        return status;
    }

    // Hardcoded ECC private key (for signing)
    uint8_t pre_provisioned_key[32] = {
        0xA9, 0xB4, 0x54, 0xB2, 0x6D, 0x6F, 0x90, 0xA4,
        0xEA, 0x31, 0x19, 0x35, 0x64, 0xCB, 0xA9, 0x1F,
        0xEC, 0x6F, 0x9A, 0x00, 0x2A, 0x7D, 0xC0, 0x50,
        0x4B, 0x92, 0xA1, 0x93, 0x71, 0x34, 0x58, 0x5F};

    // Import the key into PSA storage as a symmetric key for HMAC
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 256); // 256-bit key

    mbedtls_svc_key_id_t key_id;
    status = psa_import_key(&attributes, pre_provisioned_key, sizeof(pre_provisioned_key), &key_id);
    if (status != PSA_SUCCESS)
    {
        printf("ERROR: Failed to import the key. Status: %d\n", status);
        return status;
    }
    // Now sign the hash using HMAC
    uint8_t signature[SIGNATURE_BUFFER_SIZE];
    size_t signature_len;
    status = psa_sign_hash(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256), hash, hash_len, signature, sizeof(signature), &signature_len);
    if (status != PSA_SUCCESS)
    {
        LOG_INFFMT("[Secure] ERROR: Failed to sign the report with HMAC.\n");
        return status;
    }

    QCBOREncode_Init(&encode_ctx, buffer);
    QCBOREncode_OpenMap(&encode_ctx);
    QCBOREncode_AddBytesToMap(&encode_ctx, "ia_report", (UsefulBufC){token_buf, token_size});
    QCBOREncode_AddUInt64ToMap(&encode_ctx, "faddr", (uint64_t)faddr);
    QCBOREncode_AddInt64ToMap(&encode_ctx, "execution_output", (int64_t)execution_output);
    // QCBOREncode_AddBytesToMap(&encode_ctx, "signature", (UsefulBufC){signature, signature_len});
    QCBOREncode_CloseMap(&encode_ctx);

    err = QCBOREncode_Finish(&encode_ctx, &encoded);
    if (err != QCBOR_SUCCESS)
    {
        LOG_INFFMT("[Secure] ERROR: Failed to encode final CBOR report. QCBOR Error: %d\n", err);
        return PSA_ERROR_GENERIC_ERROR;
    }

    *cbor_report_len = encoded.len;
    return PSA_SUCCESS;
}
