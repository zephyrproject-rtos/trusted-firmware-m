#ifndef ATTEST_EXECUTE_H
#define ATTEST_EXECUTE_H
#include "tfm_sp_log.h" // TF-M Secure Partition Logging
#include <stdint.h>

typedef int (*ns_function_ptr_t)(void) __attribute__((cmse_nonsecure_call));

/**
 * @brief Function execution in Secure World via Function Address.
 *
 * @param faadr    Function Address
 */
int ns_execute(uintptr_t faddr);

#endif // POX_EXECUTE_H
