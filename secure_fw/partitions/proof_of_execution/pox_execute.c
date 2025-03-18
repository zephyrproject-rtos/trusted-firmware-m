#include "pox_execute.h"
#include "tfm_sp_log.h" // TF-M Secure Partition Logging
#include <stdint.h>

// Define the function pointer type for non-secure functions
typedef int (*ns_function_ptr_t)(void) __attribute__((cmse_nonsecure_call));

// Implementation of execute_function
int pox_execute(uintptr_t faddr)
{
    ns_function_ptr_t ns_func = (ns_function_ptr_t)faddr;
    if (!ns_func)
    {
        LOG_ERRFMT("[Secure] ERROR: Invalid function address.\n");
        return -1;
    }

    // Execute and return the output
    return ns_func();
}
