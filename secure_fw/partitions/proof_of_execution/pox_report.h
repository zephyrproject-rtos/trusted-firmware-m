#ifndef POX_REPORT_H
#define POX_REPORT_H

#include "pox_report.h"
#include "psa/initial_attestation.h"
#include "psa/crypto.h"
#include <stdio.h>
#include "tfm_sp_log.h"

#define CBOR_BUFFER_SIZE 1024
#define ATT_MAX_TOKEN_SIZE 0x240  // Adjusted for max token size

/**
 * @brief Retrieves the Initial Attestation Token (IAT).
 *
 * @param challenge    Pointer to the challenge (32 bytes).
 * @param token_buf    Buffer to store the attestation token.
 * @param token_size   Pointer to store the actual size of the token.
 * @return psa_status_t  PSA_SUCCESS on success, error code otherwise.
 */
psa_status_t att_get_iat(uint8_t *challenge, uint8_t *token_buf, size_t *token_size);

/**
 * @brief Generates the Proof of Execution (PoX) report in CBOR format.
 *
 * @param token_buf       Pointer to the attestation token.
 * @param token_size      Size of the attestation token.
 * @param faddr           Function address that was executed.
 * @param execution_output Result of the function execution.
 * @param cbor_report     Buffer to store the CBOR-encoded report.
 * @param cbor_report_len Pointer to store the actual length of the report.
 * @return psa_status_t   PSA_SUCCESS on success, error code otherwise.
 */
psa_status_t generate_pox_report(uint8_t *token_buf, size_t token_size, uintptr_t faddr, int execution_output, uint8_t *cbor_report, size_t *cbor_report_len);

#endif // POX_REPORT_H
