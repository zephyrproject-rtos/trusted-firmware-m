#include "psa_manifest/tfm_pox.h"
#include "pox_handler.h"
#include "psa/service.h"
#include "tfm_sp_log.h" // TF-M Secure Partition Logging

// Secure partition entry point that continuously listens for requests
void pox_ipc_entry(void)
{
    psa_signal_t signals = 0;

    LOG_INFFMT("[INF][POX] POX partition Initialize!\n");

    while (1)
    {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        LOG_INFFMT("[Secure] Received signal: 0x%X\n", signals);
        
        if (signals & TFM_POX_SERVICE_SIGNAL)
        {
            psa_msg_t msg;
            psa_status_t status = psa_get(TFM_POX_SERVICE_SIGNAL, &msg);
            if (status != PSA_SUCCESS)
            {
                printf("[Secure] ERROR: psa_get() failed with status: %d\n", status);
                psa_panic();
            }

            // Handle the received message with the PoX handler
            pox_ipc_handler(&msg);
        }
        else
        {
            printf("[Secure] ERROR: Unexpected signal received (0x%X)\n", signals);
            psa_panic();
        }
    }
}
