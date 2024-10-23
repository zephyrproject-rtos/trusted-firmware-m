/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "tfm_plat_otp.h"
#include "pico/bootrom.h"
#include "hardware/regs/otp_data.h"
#include <string.h>

#define OTP_BUFFER_MASK 0x0000FFFF
#define OTP_IS_WRITE_MASK 0x00010000
#define OTP_IS_ECC_MASK 0x00020000
#define OTP_ROW_PER_PAGE 0x40

struct rp2350_otp_element_t {
    uint16_t row_offset; /* OTP row offset, used in otp_access() bootrom api */
    uint8_t byte_len; /* Length of the element in bytes, should be aligned(2) or
                    aligned(4) depending usage mode */
};

/* RP2350 OTP is accessable through the bootrom api: otp_access. This OTP is
   organized into pages and rows, it has 64 pages, each 64 rows in size, 4096
   rows altogether. Each row has 24 bits (16 bit data, 8bit ECC). Because of its
   structure accesses have to be aligned to 2 bytes. The default OTP elements
   need to be placed here. This sturct serves as a map between rows and
   tfm_otp_element_id_t ids.
   The first 3 pages are occupied.
*/
static const struct rp2350_otp_element_t otp_map[] = {
    [PLAT_OTP_ID_HUK] =                      {.row_offset = 0xC0 + 0x00 , .byte_len = 32 },
    [PLAT_OTP_ID_GUK] =                      {.row_offset = 0xC0 + 0x10 , .byte_len = 32 },
    [PLAT_OTP_ID_IAK] =                      {.row_offset = 0xC0 + 0x20 , .byte_len = 32 },
    [PLAT_OTP_ID_IAK_LEN] =                  {.row_offset = 0xC0 + 0x30 , .byte_len = 4  },
    [PLAT_OTP_ID_IAK_TYPE] =                 {.row_offset = 0xC0 + 0x32 , .byte_len = 4  },
    [PLAT_OTP_ID_IAK_ID] =                   {.row_offset = 0xC0 + 0x34 , .byte_len = 32 },
    [PLAT_OTP_ID_BOOT_SEED] =                {.row_offset = 0xC0 + 0x44 , .byte_len = 32 },
    [PLAT_OTP_ID_LCS] =                      {.row_offset = 0xC0 + 0x54 , .byte_len = 4  },
    [PLAT_OTP_ID_IMPLEMENTATION_ID] =        {.row_offset = 0xC0 + 0x56 , .byte_len = 32 },
    [PLAT_OTP_ID_CERT_REF] =                 {.row_offset = 0xC0 + 0x66 , .byte_len = 32 },
    [PLAT_OTP_ID_VERIFICATION_SERVICE_URL] = {.row_offset = 0xC0 + 0x76 , .byte_len = 32 },
    [PLAT_OTP_ID_PROFILE_DEFINITION] =       {.row_offset = 0xC0 + 0x86 , .byte_len = 32 },
    [PLAT_OTP_ID_BL2_ROTPK_0] =              {.row_offset = 0xC0 + 0x96 , .byte_len = 100},
    [PLAT_OTP_ID_BL2_ROTPK_1] =              {.row_offset = 0xC0 + 0xC8 , .byte_len = 100},
    [PLAT_OTP_ID_BL2_ROTPK_2] =              {.row_offset = 0xC0 + 0xFA , .byte_len = 100},
    [PLAT_OTP_ID_BL2_ROTPK_3] =              {.row_offset = 0xC0 + 0x12C, .byte_len = 100},
    [PLAT_OTP_ID_NV_COUNTER_BL2_0] =         {.row_offset = 0xC0 + 0x15E, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_BL2_1] =         {.row_offset = 0xC0 + 0x17E, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_BL2_2] =         {.row_offset = 0xC0 + 0x19E, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_BL2_3] =         {.row_offset = 0xC0 + 0x1BE, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_NS_0] =          {.row_offset = 0xC0 + 0x1DE, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_NS_1] =          {.row_offset = 0xC0 + 0x1FE, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_NS_2] =          {.row_offset = 0xC0 + 0x21E, .byte_len = 64 },
    [PLAT_OTP_ID_KEY_BL2_ENCRYPTION] =       {.row_offset = 0xC0 + 0x23E, .byte_len = 0  },
    [PLAT_OTP_ID_BL1_2_IMAGE] =              {.row_offset = 0xC0 + 0x23E, .byte_len = 0  },
    [PLAT_OTP_ID_BL1_2_IMAGE_HASH] =         {.row_offset = 0xC0 + 0x23E, .byte_len = 0  },
    [PLAT_OTP_ID_BL2_IMAGE_HASH] =           {.row_offset = 0xC0 + 0x23E, .byte_len = 0  },
    [PLAT_OTP_ID_BL1_ROTPK_0] =              {.row_offset = 0xC0 + 0x23E, .byte_len = 0  },
    [PLAT_OTP_ID_NV_COUNTER_BL1_0] =         {.row_offset = 0xC0 + 0x23E, .byte_len = 0  },
    [PLAT_OTP_ID_ENTROPY_SEED] =             {.row_offset = 0xC0 + 0x23E, .byte_len = 64 },
    [PLAT_OTP_ID_SECURE_DEBUG_PK] =          {.row_offset = 0xC0 + 0x25E, .byte_len = 32 },
    [PLAT_OTP_ID_NV_COUNTER_PS_0] =          {.row_offset = 0xC0 + 0x26E, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_PS_1] =          {.row_offset = 0xC0 + 0x28E, .byte_len = 64 },
    [PLAT_OTP_ID_NV_COUNTER_PS_2] =          {.row_offset = 0xC0 + 0x2AE, .byte_len = 64 },
};

enum tfm_plat_err_t tfm_plat_otp_init(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_read(enum tfm_otp_element_id_t id,
                                      size_t out_len, uint8_t *out)
{
    otp_cmd_t row_and_flags;
    otp_cmd_t odd_byte_row_and_flags;
    int rc = 0;
    size_t out_len_checked;
    uint8_t odd_byte_buff[2] = {0};
    uint8_t *odd_byte_p;


    if ((out_len == 0) || (otp_map[id].byte_len == 0)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (id >= PLAT_OTP_ID_MAX) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Output buffer can be bigger than the OTP element */
    out_len_checked = (out_len < otp_map[id].byte_len) ? out_len : otp_map[id].byte_len;

    /* Assemble command */
    row_and_flags.flags = (OTP_BUFFER_MASK & otp_map[id].row_offset);
    /* For LCS ECC is not used so it can be updated */
    if (id != PLAT_OTP_ID_LCS) {
        row_and_flags.flags |= OTP_IS_ECC_MASK;
    }

    /* Read OTP through API */
    /* Bootrom API requires 2 byte alignment with ECC mode ON, handle odd byte separately */
    if (out_len_checked % 2) {
        /* Update len to be even */
        out_len_checked -= 1;
        /* Assemble the command for the odd byte, row number is incremented by (len in byte)/2 */
        odd_byte_row_and_flags.flags = row_and_flags.flags + (out_len_checked / 2);
        /* Set pointer to the last byte of the output (not the buffer) */
        odd_byte_p = out + out_len_checked;

        rc = rom_func_otp_access(&odd_byte_buff[0], 2, odd_byte_row_and_flags);
        if (rc) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        memcpy(odd_byte_p, &odd_byte_buff[0], 1);
    }
    if (out_len_checked) {
        rc = rom_func_otp_access(out, out_len_checked, row_and_flags);
        if (rc) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;

}

enum tfm_plat_err_t tfm_plat_otp_write(enum tfm_otp_element_id_t id,
                                       size_t in_len, const uint8_t *in)
{
    otp_cmd_t row_and_flags;
    otp_cmd_t odd_byte_row_and_flags;
    int rc = 0;
    size_t in_len_checked;
    uint8_t odd_byte_buff[2] = {0};
    uint8_t *odd_byte_p;

    if ((in_len == 0) || (otp_map[id].byte_len == 0)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (id >= PLAT_OTP_ID_MAX) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    in_len_checked = (in_len < otp_map[id].byte_len) ? in_len : otp_map[id].byte_len;

    /* Assemble command */
    row_and_flags.flags =  OTP_IS_WRITE_MASK |
                    (OTP_BUFFER_MASK & otp_map[id].row_offset);
    /* For LCS ECC is not used so it can be updated */
    if (id != PLAT_OTP_ID_LCS) {
        row_and_flags.flags |= OTP_IS_ECC_MASK;
    }

    /* Write OTP through API */
    /* Bootrom API requires 2 byte alignment with ECC mode ON, handle odd byte separately */
    if (in_len_checked % 2) {
        /* Update len to be even */
        in_len_checked -= 1;
        /* Assemble the command for the odd byte, row number is incremented by (len in byte)/2 */
        odd_byte_row_and_flags.flags = row_and_flags.flags + (in_len_checked / 2);
        /* Set pointer to the last byte of the input (not the buffer) */
        odd_byte_p = in + in_len_checked;
        memcpy(&odd_byte_buff[0], odd_byte_p, 1);

        rc = rom_func_otp_access(&odd_byte_buff[0], 2, odd_byte_row_and_flags);
        if (rc) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    if (in_len_checked) {
        rc = rom_func_otp_access(in, in_len_checked, row_and_flags);
        if (rc) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_get_size(enum tfm_otp_element_id_t id,
                                          size_t *size)
{
    if (id >= PLAT_OTP_ID_MAX) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    *size = otp_map[id].byte_len;
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_secure_provisioning_start(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_secure_provisioning_finish(void)
{

    uint32_t row_count = 0;
    uint8_t first_page_to_lock;
    uint8_t last_page_to_lock;
    uint8_t num_pages_to_lock;
    otp_cmd_t row_and_flags;
    uint8_t msg_buff[4] = {0};
    uint32_t lock_config;
    int rc = 0;

    /* Count the number of allocated OTP rows, assuming continious usage */
    for (int i = 0; i <= PLAT_OTP_ID_SECURE_DEBUG_PK; i++) {
        row_count += otp_map[i].byte_len;
    }

    /* Get the pages to be locked */
    first_page_to_lock = otp_map[0].row_offset / OTP_ROW_PER_PAGE;

    last_page_to_lock = (otp_map[PLAT_OTP_ID_SECURE_DEBUG_PK].row_offset +
                         (otp_map[PLAT_OTP_ID_SECURE_DEBUG_PK].byte_len / 2)) /
                        0x40;
    num_pages_to_lock = last_page_to_lock - first_page_to_lock + 1;

    /* First and last 3 pages are already in use */
    if ((first_page_to_lock < 3) || (last_page_to_lock > 60)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Assmble message */
    /* Lock information encoded to the first 8 bits of lock register 1 */
    lock_config = (OTP_DATA_PAGE0_LOCK1_LOCK_NS_BITS &
                   (OTP_DATA_PAGE0_LOCK1_LOCK_NS_VALUE_INACCESSIBLE <<
                    OTP_DATA_PAGE0_LOCK1_LOCK_NS_LSB)) |
                  (OTP_DATA_PAGE0_LOCK1_LOCK_BL_BITS &
                   (OTP_DATA_PAGE0_LOCK1_LOCK_BL_VALUE_INACCESSIBLE <<
                    OTP_DATA_PAGE0_LOCK1_LOCK_BL_LSB));
    /* Triple majority vote */
    msg_buff[0] = lock_config;
    msg_buff[1] = lock_config;
    msg_buff[2] = lock_config;

    /* Lock pages, NS and BL code should not be able to access them */
    for (int i = 0; i < num_pages_to_lock; i++) {
        /* Assemble command */
        row_and_flags.flags = OTP_IS_WRITE_MASK |
                              (OTP_BUFFER_MASK & (OTP_DATA_PAGE0_LOCK1_ROW +
                               ((i + first_page_to_lock) * 2)));

        /* Write lock register PAGExx_LOCK1 */
        rc = rom_func_otp_access(&msg_buff[0], 4, row_and_flags);
        if (rc) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}
