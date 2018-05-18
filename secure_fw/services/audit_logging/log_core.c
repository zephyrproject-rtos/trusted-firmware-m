/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "log_core.h"
#include "tfm_log_defs.h"
#include "tfm_secure_api.h"

/*!
 * \def LOG_UART_REDIRECTION
 *
 * \brief If set to 1 by the build system, UART redirection is enabled. Keep it
 *        disabled by default.
 */
#ifndef LOG_UART_REDIRECTION
#define LOG_UART_REDIRECTION (0U)
#endif

#if (LOG_UART_REDIRECTION == 1U)
/* CMSIS Driver for UART */
#include "Driver_USART.h"

#ifndef LOG_UART_NAME
/* Default secure UART name */
#define LOG_UART_NAME Driver_USART1
#endif
extern ARM_DRIVER_USART LOG_UART_NAME;

/*!
 * \def LOG_UART_BAUD_RATE
 *
 * \brief The baud rate used when redirecting the log entry on the secure UART.
 *        Default value is 115200.
 */
#ifndef LOG_UART_BAUD_RATE
#define LOG_UART_BAUD_RATE (115200)
#endif

/*!
 * \var log_uart_init_success
 *
 * \brief This variable is 0 in case UART init has failed during service
 *        initialization, 1 otherwise.
 */
static uint8_t log_uart_init_success = 0U;

/*!
 * \var hex_values
 *
 * \brief Array variable used to translate binary to ASCII character
 *        representation for UART redirection
 */
static const char hex_values[] = "0123456789ABCDEF";
#endif

/*!
 * \def MEMBER_SIZE
 *
 * \brief Standard macro to get size of elements in a struct
 */
#define MEMBER_SIZE(type,member) sizeof(((type *)0)->member)

/*!
 * \def LOG_FIXED_FIELD_SIZE
 *
 * \brief Size of the mandatory header fields that are before the info received
 *        from the client partition, i.e.
 *        [TIMESTAMP][IV_COUNTER][PARTITION_ID][SIZE]
 */
#define LOG_FIXED_FIELD_SIZE (MEMBER_SIZE(struct log_hdr, timestamp) + \
                              MEMBER_SIZE(struct log_hdr, iv_counter) + \
                              MEMBER_SIZE(struct log_hdr, partition_id) + \
                              MEMBER_SIZE(struct log_hdr, size))
/*!
 * \def LOG_SIZE
 *
 * \brief Size of the allocated space for the log, in bytes
 *
 * \note Must be a multiple of 8 bytes.
 */
#define LOG_SIZE (1024)

/*!
 * \def MIN(a,b)
 *
 * \brief A standard MIN macro
 *
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/*!
 * \var log_buffer
 *
 * \brief The private buffer containing the the log in memory
 *
 * \note Aligned to 4 bytes to keep the wrapping on a 4-byte aligned boundary
 */
__attribute__ ((aligned(4)))
static uint8_t log_buffer[LOG_SIZE] = {0};

/*!
 * \var scratch_buffer
 *
 * \brief Scratch buffers needed to hold plain text (and encrypted, if
 *        available) log items to be added
 */
static uint64_t scratch_buffer[(LOG_SIZE)/8] = {0};

/*!
 * \struct log_vars
 *
 * \brief Contains the state variables associated to the current state of the
 *        audit log
 */
struct log_vars {
    uint32_t first_el_idx; /*!< Index in the log of the first element
                                in chronological order */
    uint32_t last_el_idx;  /*!< Index in the log of the last element
                                in chronological order */
    uint32_t num_items;    /*!< Indicates the number of items
                                currently stored in the log. It has to be
                                zero after a reset, i.e. log is empty */
    uint32_t stored_size;  /*!< Indicates the total size of the items
                                currently stored in the log */
};

/*!
 * \var log_state
 *
 * \brief Current state variables for the log
 */
static struct log_vars log_state = {0};

/*!
 * \var global_timestamp
 *
 * \brief Used to have a progressive number attached to each log entry, will be
 *        replaced with a proper timestamping request to TF-M when available
 *
 * \note This is out of the log_state because this is just used to mock up a
 *       timestamping and will be removed later when the final timestamping
 *       mechanism is in place
 */
static uint64_t global_timestamp = 0;

/*!
 * \def DUMMY_PARTITION_ID
 *
 * \brief Set to 1234, this will be changed when the TFM unpriviliged API to
 *        return the partition ID of the caller will be ready
 */
#define DUMMY_PARTITION_ID (1234)

/*!
 * \brief Static inline function to get the log buffer ptr from index
 *
 * \param[in] idx Byte index to be converted to pointer in the log buffer
 *
 * \return Pointer at the beginning of the log item in the log buffer
 */
__attribute__ ((always_inline)) __STATIC_INLINE
struct log_hdr *GET_LOG_POINTER(const uint32_t idx)
{
    return (struct log_hdr *)( &log_buffer[idx] );
}

/*!
 * \brief Static inline function to get the pointer to the SIZE field
 *
 * \param[in] idx Byte index to which retrieve the corresponding size pointer
 *
 * \return Pointer to the size field in the log item header
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t *GET_SIZE_FIELD_POINTER(const uint32_t idx)
{
    return (uint32_t *) GET_LOG_POINTER( (idx+offsetof(struct log_hdr, size))
                                         % LOG_SIZE );
}

/*!
 * \brief Static inline function to compute the full log entry size starting
 *        from the value of the size field
 *
 * \param[in] size Size of the log line from which derive the size of the whole
 *                 log item
 *
 * \return Full log item size
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t COMPUTE_LOG_ENTRY_SIZE(const uint32_t size)
{
    return (LOG_FIXED_FIELD_SIZE + size + LOG_MAC_SIZE);
}

/*!
 * \brief Static inline function to get the index to the base of the log buffer
 *        for the next item with respect to the current item
 *
 * \param[in] idx Byte index of the current item in the log
 *
 * \return Index of the next item in the log
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t GET_NEXT_LOG_INDEX(const uint32_t idx)
{
    return (uint32_t) ( (idx + COMPUTE_LOG_ENTRY_SIZE(
                                   *GET_SIZE_FIELD_POINTER(idx)) ) % LOG_SIZE );
}

/*!
 * \brief Static function to update the state variables of the log after the
 *        addition of a new log line of a given size
 *
 * \param[in] first_el_idx First element index
 * \param[in] last_el_idx  Last element index
 * \param[in] stored_size  New value of the stored size
 * \param[in] num_items    Number of elements stored
 *
 */
static void log_update_state(const uint32_t first_el_idx,
                             const uint32_t last_el_idx,
                             const uint32_t stored_size,
                             const uint32_t num_items)
{
    /* Update the indexes */
    log_state.first_el_idx = first_el_idx;
    log_state.last_el_idx = last_el_idx;

    /* Update the items*/
    log_state.num_items = num_items;

    /* Update the size of the stored items */
    log_state.stored_size = stored_size;
}

/*!
 * \brief Static function to identify the begin and end position for a new write
 *        into the log. It will replace items based on "older entries first"
 *        policy in case not enough space is available in the log
 *
 * \param[in]  size  Size of the line we need to fit
 * \param[out] begin Pointer to the index to begin
 * \param[out] end   Pointer to the index to end
 *
 */
static void log_replace_item(const uint32_t size,
                             uint32_t *begin,
                             uint32_t *end)
{
    uint32_t first_el_idx = 0, last_el_idx = 0;
    uint32_t num_items = 0, stored_size = 0;
    uint32_t start_pos = 0, stop_pos = 0;

    /* Retrieve the current state variables of the log */
    first_el_idx = log_state.first_el_idx;
    last_el_idx = log_state.last_el_idx;
    num_items = log_state.num_items;
    stored_size = log_state.stored_size;

    /* If there is not enough size, remove older entries */
    while (size > (LOG_SIZE - stored_size)) {

        /* In case we did a full loop without finding space, reset */
        if (num_items == 0) {
            first_el_idx = 0;
            last_el_idx = 0;
            num_items = 0;
            stored_size = 0;
            break;
        }

        /* Remove the oldest */
        stored_size -= COMPUTE_LOG_ENTRY_SIZE(
                           *GET_SIZE_FIELD_POINTER(first_el_idx) );
        num_items--;
        first_el_idx = GET_NEXT_LOG_INDEX(first_el_idx);
    }

    /* Get the start and stop positions */
    if (num_items == 0) {
        start_pos = first_el_idx;
    } else {
        start_pos = GET_NEXT_LOG_INDEX(last_el_idx);
    }
    stop_pos = ((start_pos + COMPUTE_LOG_ENTRY_SIZE(size)) % LOG_SIZE);

    /* Return begin and end positions */
    *begin = start_pos;
    *end = stop_pos;

    /* Update the state with the new values of variables */
    log_update_state(first_el_idx, last_el_idx, stored_size, num_items);
}

/*!
 * \brief Static function to perform memory copying into the log buffer. It
 *        takes into account circular wrapping on the log buffer size.
 *
 * \param[in] dest Pointer to the destination buffer
 * \param[in] src  Pointer to the source buffer
 * \param[in] size Size in bytes to be copied
 *
 */
static enum tfm_log_err log_buffer_copy(uint8_t *dest,
                                        const uint8_t *src,
                                        const uint32_t size)
{
    uint32_t idx = 0;
    uint32_t dest_idx = (uint32_t)dest - (uint32_t)&log_buffer[0];

    if ((dest_idx >= LOG_SIZE) || (size > LOG_SIZE)) {
        return TFM_LOG_ERR_FAILURE;
    }

    /* TODO: This can be an optimized copy using uint32_t
     *       and enforcing the condition that wrapping
     *       happens only on 4-byte boundaries
     */

    for (idx = 0; idx < size; idx++) {
        log_buffer[(dest_idx + idx) % LOG_SIZE] = src[idx];
    }

    return TFM_LOG_ERR_SUCCESS;
}

/*!
 * \brief Static function to emulate memcpy
 *
 * \param[out] dest Pointer to the destination buffer
 * \param[in]  src  Pointer to the source buffer
 * \param[in]  size Size in bytes to be copied
 *
 */
static enum tfm_log_err log_memcpy(uint8_t *dest,
                                   const uint8_t *src,
                                   const uint32_t size)
{
    uint32_t idx = 0;

    for (idx = 0; idx < size; idx++) {
        dest[idx] = src[idx];
    }

    return TFM_LOG_ERR_SUCCESS;
}

/*!
 * \brief Static function to format a log entry before the addition to the log
 *
 * \param[out] buffer Pointer to the buffer to format
 * \param[in]  line   Pointer to the line to be added
 *
 */
static enum tfm_log_err log_format_buffer(uint64_t *buffer,
                                          const struct tfm_log_line *line)
{
    struct log_hdr *hdr = NULL;
    struct log_tlr *tlr = NULL;
    uint32_t size, idx;

    /* Get the size from the log line */
    size = line->size;

    /* Format the scratch buffer with the complete log item */
    hdr = (struct log_hdr *) buffer;

    /* FIXME: Timestamping/IV counter and partition ID retrieval through
     *        secure function to function calls and TFM API - not available yet.
     */
    hdr->timestamp = global_timestamp++;
    hdr->iv_counter = 0;
    hdr->partition_id = DUMMY_PARTITION_ID;

    /* Copy the log line into the scratch buffer */
    log_memcpy( (uint8_t *) &(hdr->size),
                (const uint8_t *) line,
                size+4 );

    /* FIXME: The MAC here is just a dummy value for prototyping. It will be
     *        filled by a call to the crypto interface directly when available.
     */
    tlr = (struct log_tlr *) ((uint8_t *)hdr + LOG_FIXED_FIELD_SIZE + size);
    for (idx=0; idx<LOG_MAC_SIZE; idx++) {
        tlr->mac[idx] = idx;
    }

    return TFM_LOG_ERR_SUCCESS;
}

/*!
 * \brief Static function to stream an entry of the log to a (secure) UART
 *
 * \details The entry of the log is streamed as a stream of hex values,
 *          not parsed nor interpreted.
 *
 * \param[in] start_idx Byte index in the log from where to start streaming
 *            to UART
 *
 */
static void log_uart_redirection(const uint32_t start_idx)
{
#if (LOG_UART_REDIRECTION == 1U)
    uint32_t size = *GET_SIZE_FIELD_POINTER(start_idx);
    uint8_t end_of_line[] = {'\r', '\n'};
    uint32_t idx = 0;
    uint8_t read_byte;

    if (log_uart_init_success == 1U) {
        for (idx=0; idx<COMPUTE_LOG_ENTRY_SIZE(size); idx++) {
            read_byte = log_buffer[(start_idx+idx) % LOG_SIZE];
            (void)LOG_UART_NAME.Send(&hex_values[(read_byte >> 4) & 0xF],1);
            (void)LOG_UART_NAME.Send(&hex_values[read_byte & 0xF], 1);
            (void)LOG_UART_NAME.Send(" ", 1);
        }
        (void)LOG_UART_NAME.Send(&end_of_line, 2);
    }
#endif
}

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
enum tfm_log_err log_core_init(void)
{
#if (LOG_UART_REDIRECTION == 1U)
    int32_t ret = ARM_DRIVER_OK;

    ret = LOG_UART_NAME.Initialize(NULL);
    if (ret != ARM_DRIVER_OK) {
        return TFM_LOG_ERR_FAILURE;
    }

    ret = LOG_UART_NAME.Control(ARM_USART_MODE_ASYNCHRONOUS,
                                LOG_UART_BAUD_RATE);
    if (ret != ARM_DRIVER_OK) {
        return TFM_LOG_ERR_FAILURE;
    }

    /* If we get to this point, UART init is successful */
    log_uart_init_success = 1U;
#endif

    /* Clear the log state variables */
    log_update_state(0,0,0,0);

    return TFM_LOG_ERR_SUCCESS;
}

enum tfm_log_err log_core_delete_items(const uint32_t num_items,
                                       uint32_t *rem_items)
{
    uint32_t first_el_idx = 0, idx = 0;

    if (rem_items == NULL) {
        return TFM_LOG_ERR_FAILURE;
    }

    /* This means to delete all items in the log */
    if (num_items >= log_state.num_items) {

        /* Update the number of removed items (all of them) */
        *rem_items = log_state.num_items;

        /* Clear the log state variables */
        log_update_state(0,0,0,0);

        return TFM_LOG_ERR_SUCCESS;
    }

    /* Get the index of the first element */
    first_el_idx = log_state.first_el_idx;

    /* Removing items means discarding items at the head */
    for (idx = 0; idx < num_items; idx++) {
        first_el_idx = GET_NEXT_LOG_INDEX(first_el_idx);
    }

    /* Update the state with the new head and number of items */
    log_state.first_el_idx = first_el_idx;
    log_state.num_items -= num_items;

    /* Update the number of removed items */
    *rem_items = num_items;

    return TFM_LOG_ERR_SUCCESS;
}

enum tfm_log_err log_core_get_info(struct tfm_log_info *info)
{
    /* Return the size of the items currently stored */
    info->size = log_state.stored_size;

    /* Return the number of items that are currently stored */
    info->num_items = log_state.num_items;

    return TFM_LOG_ERR_SUCCESS;
}

enum tfm_log_err log_core_add_line(const struct tfm_log_line *line)
{
    struct tfm_log_info info;

    uint32_t start_pos = 0, stop_pos = 0;
    uint32_t first_el_idx = 0, last_el_idx = 0, size = 0;
    uint32_t num_items = 0, stored_size = 0;

    /* Check that the request comes from the secure world */
    if (tfm_core_validate_secure_caller() != TFM_SUCCESS) {
        return TFM_LOG_ERR_FAILURE;
    }

    /* Read the size from the input line */
    size = line->size;

    /* Check that size is a 4-byte multiple as expected */
    if (size % 4) {
        return TFM_LOG_ERR_FAILURE;
    }

    /* Check that the entry to be added is not greater than the
     * maximum space available
     */
    if (size > (LOG_SIZE - (LOG_FIXED_FIELD_SIZE+LOG_MAC_SIZE))) {
        return TFM_LOG_ERR_FAILURE;
    }

    /* Get the size in bytes and num of elements present in the log */
    log_core_get_info(&info);
    num_items = info.num_items;
    stored_size = info.size;

    if (num_items == 0) {

        start_pos = 0;
        stop_pos = COMPUTE_LOG_ENTRY_SIZE(size) - 1;

    } else {

        /* The log is not empty, need to decide the candidate position
         * and invalidate older entries in case there is not enough space
         */
        log_replace_item(COMPUTE_LOG_ENTRY_SIZE(size), &start_pos, &stop_pos);
    }

    /* Format the scratch buffer with the complete log item */
    log_format_buffer(&scratch_buffer[0], line);

    /* TODO: At this point, encryption should be called if supported */

    /* Do the copy of the log line to be added in the log */
    log_buffer_copy( (uint8_t *) &log_buffer[start_pos],
                     (const uint8_t *) &scratch_buffer[0],
                     COMPUTE_LOG_ENTRY_SIZE(size) );

    /* Retrieve current log state */
    first_el_idx = log_state.first_el_idx;
    num_items = log_state.num_items;
    stored_size = log_state.stored_size;

    /* The last element is the one we just added */
    last_el_idx = start_pos;

    /* Update the number of items and stored size */
    num_items++;
    stored_size += COMPUTE_LOG_ENTRY_SIZE(size);

    /* Update the log state */
    log_update_state(first_el_idx, last_el_idx, stored_size, num_items);

    /* TODO: At this point, we would need to update the stored copy in
     *       persistent storage. Need to define a strategy for this
     */

    /* Stream to a secure UART if available for the platform and built */
    log_uart_redirection(last_el_idx);

    return TFM_LOG_ERR_SUCCESS;
}

enum tfm_log_err log_core_retrieve(const uint32_t size,
                                   const int32_t start,
                                   uint8_t *buffer,
                                   struct tfm_log_info *info)
{
    uint32_t stored_size = 0, retrieved_log_size = 0;
    uint32_t index_first_empty_el = 0;
    uint32_t idx, start_idx = 0, num_items = 0;

    struct tfm_log_info info_stored;

    /* size must be a non-zero value */
    if (size == 0) {
        info->size = 0;
        info->num_items = 0;
        return TFM_LOG_ERR_FAILURE;
    }

    /* Get the size in bytes and num of elements present in the log */
    log_core_get_info(&info_stored);
    num_items = info_stored.num_items;
    stored_size = info_stored.size;

    /* Log is empty, but still a valid scenario */
    if (num_items == 0) {
        info->size = 0;
        info->num_items = 0;
        return TFM_LOG_ERR_SUCCESS;
    }

    /* Compute the size in bytes to be retrieved */
    retrieved_log_size = MIN(size, stored_size);

    /* First element to read from the log */
    start_idx = log_state.first_el_idx;

    if (start == TFM_LOG_READ_RECENT) {

        /* Get the index of the first empty location */
        index_first_empty_el = GET_NEXT_LOG_INDEX(log_state.last_el_idx);

        /* If the stored log size is bigger than what we are able to retrieve,
         * just return the latest entries that fit into the available space
         */
        while (retrieved_log_size < stored_size) {

            start_idx = GET_NEXT_LOG_INDEX(start_idx);

            /* Decrement the number of items that we will return */
            num_items--;

            stored_size = (index_first_empty_el >= start_idx) ?
                          (index_first_empty_el - start_idx) :
                          (LOG_SIZE - start_idx) + index_first_empty_el;

            if (stored_size < retrieved_log_size) {
                /* The retrieved log size now will be the new stored log size */
                retrieved_log_size = stored_size;
            }
        }

        /* size available is not enough even to retrieve a single log entry */
        if (stored_size == 0) {
            info->size = 0;
            info->num_items = 0;
            return TFM_LOG_ERR_SUCCESS;
        }

    } else if (start < num_items) {

        /* Move the start_idx index to the desired element */
        for (idx=0; idx<start; idx++) {
            start_idx = GET_NEXT_LOG_INDEX(start_idx);
        }

        /* Initialize to the size of the first element to retrieve */
        stored_size = COMPUTE_LOG_ENTRY_SIZE(
                          *GET_SIZE_FIELD_POINTER(start_idx) );

        /* size available is not enough even to retrieve a single log entry */
        if (stored_size > retrieved_log_size) {
            info->size = 0;
            info->num_items = 0;
            return TFM_LOG_ERR_SUCCESS;
        }

        /* Initialize the value of num_items */
        num_items = 1;

        /* Compute the total size to retrieve */
        idx = GET_NEXT_LOG_INDEX(start_idx);
        while ((stored_size + COMPUTE_LOG_ENTRY_SIZE(
                    *GET_SIZE_FIELD_POINTER(idx) )) <= retrieved_log_size) {

            /* Update stored_size */
            stored_size += COMPUTE_LOG_ENTRY_SIZE(
                               *GET_SIZE_FIELD_POINTER(idx) );

            /* Increment the number of items that we will return */
            num_items++;

            /* Move to the next item to check */
            idx = GET_NEXT_LOG_INDEX(idx);
        }

        /* The retrieved log size now will be the new stored log size */
        retrieved_log_size = stored_size;

    } else {

        /* The index value is wrong */
        info->size = 0;
        info->num_items = 0;
        return TFM_LOG_ERR_FAILURE;
    }

    /* Do the copy */
    for (idx=0; idx<retrieved_log_size; idx++) {
        buffer[idx] = log_buffer[(start_idx + idx) % LOG_SIZE];
    }

    /* Update the retrieved size */
    info->size = retrieved_log_size;
    /* Update the number of items returned */
    info->num_items = num_items;

    return TFM_LOG_ERR_SUCCESS;
}
/*!@}*/
