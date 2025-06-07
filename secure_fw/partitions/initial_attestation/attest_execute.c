#include "attest_execute.h"
#include "tfm_sp_log.h"
#include <stdint.h>

int ns_execute(uintptr_t faddr)
{
    if (!faddr)
    {
        LOG_ERRFMT("[Secure] ERROR: Null function pointer.\n");
        return -1;
    }

    // Cast to non-secure function pointer with cmse_nonsecure_call attribute
    ns_function_ptr_t ns_function = (ns_function_ptr_t)(faddr | 1U); // Set bit 0 for Thumb mode

    // Call the non-secure function
    int ret = ns_function();

    LOG_INFFMT("[Secure] INFO: Non-secure function returned: %d\n", ret);
    return ret;
}