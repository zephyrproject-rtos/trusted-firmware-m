/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SPE_MAILBOX_H__
#define __TFM_SPE_MAILBOX_H__

#include "tfm_mailbox.h"
#include "tfm_hal_mailbox.h"

/**
 * \brief Handle mailbox message(s) from NSPE.
 *
 * \retval MAILBOX_SUCCESS      Successfully get PSA client call return result.
 * \retval Other return code    Operation failed with an error code.
 */
int32_t tfm_mailbox_handle_msg(void);

/**
 * \brief Return PSA client call return result to NSPE.
 *
 * \param[in] handle            The handle to the mailbox message
 * \param[in] reply             PSA client call return result to be written
 *                              to NSPE.
 *
 * \retval MAILBOX_SUCCESS      Operation succeeded.
 * \retval Other return code    Operation failed with an error code.
 */
int32_t tfm_mailbox_reply_msg(mailbox_msg_handle_t handle, int32_t reply);

#endif /* __TFM_SPE_MAILBOX_H__ */
