#include "pox_report.h"
#include "attest.h"
#include "attest_token.h"
#include "psa/crypto.h"
#include "tfm_sp_log.h" // TF-M Secure Partition Logging
#include "psa/initial_attestation.h"
#include <string.h> // For memcpy
#include "tfm_crypto_defs.h"
#include "tfm_attest_iat_defs.h"

#define SIGNATURE_BUFFER_SIZE 64

// Structure to store formatting options
struct sf_hex_tbl_fmt
{
    bool ascii;      // Flag to include ASCII representation
    bool addr_label; // Flag to show address labels
    uint32_t addr;   // Starting address
};

static enum psa_attest_err_t attest_get_t_cose_algorithm(int32_t *cose_algorithm_id);

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
    print_hex(&fmt, token_buf, *sys_token_sz); // Only print the actual token size, not the whole buffer
    return PSA_SUCCESS;
}

psa_status_t generate_pox_report(uint8_t *token_buf, size_t token_size, uintptr_t faddr, int execution_output, uint8_t *cbor_report, size_t *cbor_report_len)
{
    if (!token_buf || !cbor_report || !cbor_report_len)
    {
        LOG_ERRFMT("[Secure] ERROR: Null pointer argument in generate_pox_report.\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Validate buffer size is sufficient
    if (*cbor_report_len < CBOR_BUFFER_SIZE)
    {
        LOG_ERRFMT("[Secure] ERROR: Output buffer too small (provided: %d, required: %d).\n",
                   *cbor_report_len, CBOR_BUFFER_SIZE);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    enum psa_attest_err_t attest_err = PSA_ATTEST_ERR_SUCCESS;
    struct attest_token_encode_ctx pox_ctx;
    struct q_useful_buf out_buf;
    struct q_useful_buf_c final_report;
    int32_t key_select = 0;
    int32_t cose_algorithm_id = 0; // Default value

    // Initialize the output buffer - ensuring it's properly set up for the CBOR operations
    out_buf.ptr = cbor_report;
    out_buf.len = *cbor_report_len;

    // Check that the buffer is valid
    if (out_buf.ptr == NULL)
    {
        LOG_ERRFMT("[Secure] ERROR: Output buffer pointer is NULL\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    LOG_INFFMT("[Secure] Output buffer initialized at %p with size: %d\n",
               out_buf.ptr, (int)out_buf.len);

    // Get the COSE algorithm ID for token signing
    attest_err = attest_get_t_cose_algorithm(&cose_algorithm_id);
    if (attest_err != PSA_ATTEST_ERR_SUCCESS)
    {
        LOG_ERRFMT("[Secure] ERROR: Failed to get COSE algorithm ID: %d\n", attest_err);
        return attest_err;
    }
    LOG_INFFMT("[Secure] COSE algorithm ID: %d\n", cose_algorithm_id);
    LOG_INFFMT("[Secure] Starting CBOR encoding with key_select: %d\n", key_select);
    enum attest_token_err_t err = attest_token_encode_start(&pox_ctx,
                                                            key_select,
                                                            cose_algorithm_id,
                                                            &out_buf);

    LOG_INFFMT("[Secure] CBOR encoding started\n");

    if (err != ATTEST_TOKEN_ERR_SUCCESS)
    {
        LOG_ERRFMT("[PoX] Failed to start encoding: %d\n", err);
        return PSA_ERROR_GENERIC_ERROR;
    }
    // // Label definitions for the PoX report
    // const int LABEL_ATTESTATION_TOKEN = 11;
    // const int LABEL_EXECUTABLE_ADDR = 12;
    // const int LABEL_EXECUTION_RESULT = 13;

    // Create a properly structured token buffer for CBOR
    struct q_useful_buf_c token_buf_c;
    token_buf_c.ptr = token_buf;
    token_buf_c.len = token_size;

    LOG_INFFMT("[Secure] Adding attestation token of size: %d\n", token_size);

    // Add the attestation token as a binary string
    attest_token_encode_add_bstr(&pox_ctx, IAT_POX_IA, &token_buf_c);

    // Verify CBOR encoding state after adding attestation token
    QCBORError cbor_error = QCBOREncode_GetErrorState(attest_token_encode_borrow_cbor_cntxt(&pox_ctx));
    if (cbor_error != QCBOR_SUCCESS)
    {
        LOG_ERRFMT("[PoX] CBOR error after adding attestation token: %d\n", cbor_error);
        return PSA_ERROR_GENERIC_ERROR;
    }
    LOG_INFFMT("[Secure] Successfully added attestation token\n");

    LOG_INFFMT("[Secure] Adding executable address: 0x%x\n", (unsigned int)faddr);
    attest_token_encode_add_integer(&pox_ctx, IAT_POX_FADDR, (int64_t)faddr);

    // Verify CBOR encoding state after adding executable address
    cbor_error = QCBOREncode_GetErrorState(attest_token_encode_borrow_cbor_cntxt(&pox_ctx));
    if (cbor_error != QCBOR_SUCCESS)
    {
        LOG_ERRFMT("[PoX] CBOR error after adding executable address: %d\n", cbor_error);
        return PSA_ERROR_GENERIC_ERROR;
    }
    LOG_INFFMT("[Secure] Successfully added executable address\n");

    LOG_INFFMT("[Secure] Adding execution result: %d\n", execution_output);
    attest_token_encode_add_integer(&pox_ctx, IAT_POX_OUT, (int64_t)execution_output);

    // Verify CBOR encoding state after adding execution result
    cbor_error = QCBOREncode_GetErrorState(attest_token_encode_borrow_cbor_cntxt(&pox_ctx));
    if (cbor_error != QCBOR_SUCCESS)
    {
        LOG_ERRFMT("[PoX] CBOR error after adding execution result: %d\n", cbor_error);
        return PSA_ERROR_GENERIC_ERROR;
    }
    LOG_INFFMT("[Secure] Finalizing CBOR encoding\n");
    err = attest_token_encode_finish(&pox_ctx, &final_report);
    if (err != ATTEST_TOKEN_ERR_SUCCESS)
    {
        LOG_ERRFMT("[PoX] Failed to finish encoding: %d\n", err);
        // Map COSE error codes to more meaningful messages
        switch (err)
        {
        case ATTEST_TOKEN_ERR_TOO_SMALL:
            LOG_ERRFMT("[PoX] Output buffer too small\n");
            return PSA_ERROR_BUFFER_TOO_SMALL;
        case ATTEST_TOKEN_ERR_CBOR_FORMATTING:
            LOG_ERRFMT("[PoX] CBOR formatting error\n");
            break;
        case ATTEST_TOKEN_ERR_SIGNING_KEY:
            LOG_ERRFMT("[PoX] Signing key error\n");
            break;
        default:
            LOG_ERRFMT("[PoX] Generic token encoding error\n");
            break;
        }
        return PSA_ERROR_GENERIC_ERROR;
    }

    // Validate final report data
    if (final_report.ptr == NULL || final_report.len == 0)
    {
        LOG_ERRFMT("[PoX] Final report is empty or invalid\n");
        return PSA_ERROR_GENERIC_ERROR;
    }

    // Check if the final report fits within the output buffer
    if (final_report.len > *cbor_report_len)
    {
        LOG_ERRFMT("[PoX] Final report size (%d) exceeds output buffer size (%d)\n",
                   (int)final_report.len, (int)*cbor_report_len);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    // Copy the final report data to the output buffer if they're not already the same
    if (final_report.ptr != cbor_report)
    {
        LOG_INFFMT("[Secure] Copying %d bytes from final report to output buffer\n",
                   (int)final_report.len);
        memcpy(cbor_report, final_report.ptr, final_report.len);
    }

    // Update the output size
    *cbor_report_len = final_report.len;

    LOG_INFFMT("[Secure] Successfully generated PoX report of size: %d\n", (int)final_report.len);

    return PSA_SUCCESS;
}

static enum psa_attest_err_t attest_get_t_cose_algorithm(
    int32_t *cose_algorithm_id)
{
    psa_status_t status;
    psa_key_attributes_t attr;
    psa_key_handle_t handle = TFM_BUILTIN_KEY_ID_IAK;
    psa_key_type_t key_type;

    status = psa_get_key_attributes(handle, &attr);
    if (status != PSA_SUCCESS)
    {
        return PSA_ATTEST_ERR_GENERAL;
    }
    key_type = psa_get_key_type(&attr);
    if (status != PSA_SUCCESS)
    {
        return PSA_ATTEST_ERR_GENERAL;
    }

    if (PSA_KEY_TYPE_IS_ECC(key_type) &&
        (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_SECP_R1))
    {
        switch (psa_get_key_bits(&attr))
        {
        case 256:
            LOG_INFFMT("ES256\n");
            *cose_algorithm_id = T_COSE_ALGORITHM_ES256;
            break;
        case 384:
            *cose_algorithm_id = T_COSE_ALGORITHM_ES384;
            break;
        case 512:
            *cose_algorithm_id = T_COSE_ALGORITHM_ES512;
            break;
        default:
            return PSA_ATTEST_ERR_GENERAL;
        }
    }
    else if (key_type == PSA_KEY_TYPE_HMAC)
    {
        switch (psa_get_key_bits(&attr))
        {
        case 256:
            LOG_INFFMT("HMAC256\n");
            *cose_algorithm_id = T_COSE_ALGORITHM_HMAC256;
            break;
        case 384:
            *cose_algorithm_id = T_COSE_ALGORITHM_HMAC384;
            break;
        case 512:
            *cose_algorithm_id = T_COSE_ALGORITHM_HMAC512;
            break;
        default:
            return PSA_ATTEST_ERR_GENERAL;
        }
    }
    else
    {
        LOG_DBGFMT("Attestation: Unexpected key_type for TFM_BUILTIN_KEY_ID_IAK. Key storage may be corrupted!\r\n");
        return PSA_ATTEST_ERR_GENERAL;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}