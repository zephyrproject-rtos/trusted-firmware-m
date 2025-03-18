#ifndef POX_HANDLER_H
#define POX_HANDLER_H

#include "psa/service.h"


// Size of the challenge in bytes (256 bits)
#define CHALLENGE_SIZE 32
#define TOKEN_BUF_SIZE 256
#define REPORT_BUF_SIZE 1024

// Function declarations
psa_status_t pox_ipc_handler(psa_msg_t *msg);

#endif // POX_HANDLER_H
