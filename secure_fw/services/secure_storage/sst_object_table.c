/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_object_table.h"

#include "assets/sst_asset_defs.h"
#include "cmsis_compiler.h"
#include "crypto/sst_crypto_interface.h"
#include "flash/sst_flash.h"
#include "sst_flash_fs.h"
#include "sst_utils.h"

/*!
 * \def SST_OBJECT_SYSTEM_VERSION
 *
 * \brief Current object system version.
 */
#define SST_OBJECT_SYSTEM_VERSION  0x01

/*!
 * \struct sst_obj_table_info_t
 *
 * \brief Object table information structure.
 */
struct sst_obj_table_entry_t {
#ifdef SST_ENCRYPTION
    uint8_t tag[SST_TAG_LEN_BYTES]; /*!< MAC value of AEAD object */
#else
    uint32_t version;               /*!< File version */
#endif
    uint32_t uuid;                  /*!< Object UUID */
};


/* Specifies number of entries in the table. The number of entries is the
 * number of assets, defined in asset_defs.h, plus one extra entry to store
 * a new object when the code processes a change in a file.
 */
#define SST_OBJ_TABLE_ENTRIES (SST_NUM_ASSETS + 1)

/*!
 * \struct sst_obj_table_t
 *
 * \brief Object table structure.
 */
struct __attribute__((__packed__)) sst_obj_table_t {
#ifdef SST_ENCRYPTION
  union sst_crypto_t crypto;      /*!< Crypto metadata. */
#endif
  uint8_t version;                /*!< SST object system version. */
  uint8_t swap_count;             /*!< Swap counter to distinguish 2 different
                                   *   object tables.
                                   */
  uint8_t reserved[2];            /*!< 32 bits alignment. */
  struct sst_obj_table_entry_t    obj_db[SST_OBJ_TABLE_ENTRIES]; /*!< Table's
                                                                  *   entries
                                                                  */
};

/* Object table indexes */
#define SST_OBJ_TABLE_IDX_0 0
#define SST_OBJ_TABLE_IDX_1 1

/* Number of object tables (active and scratch) */
#define SST_NUM_OBJ_TABLES 2

/*!
 * \def SST_TABLE_FS_ID
 *
 * \brief File ID to be used in order to store the object table in the
 *        file system.
 *
 * \param[in] idx  Table index to convert into a file ID.
 *
 * \return Returns file ID
 *
 */
#define SST_TABLE_FS_ID(idx) (idx + 1)

/*!
 * \def SST_OBJECT_FS_ID
 *
 * \brief File ID to be used in order to store an object in the
 *        file system.
 *
 * \param[in] idx  Object table index to convert into a file ID.
 *
 * \return Returns file ID
 */
#define SST_OBJECT_FS_ID(idx) ((idx + 1) + \
                                SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_1))

/*!
 * \def SST_OBJECT_FS_ID_TO_IDX
 *
 * \brief Gets object index in the table based on the file ID.
 *
 * \param[in] fid  File ID of an object in the object table
 *
 * \return Returns object table index
 */
#define SST_OBJECT_FS_ID_TO_IDX(fid) ((fid - 1) - \
                                      SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_1))

/*!
 * \struct sst_obj_table_ctx_t
 *
 * \brief Object table context structure.
 */
struct __attribute__((__packed__)) sst_obj_table_ctx_t {
    struct sst_obj_table_t obj_table; /*!< Object tables */
    uint8_t active_table;             /*!< Active object table */
    uint8_t scratch_table;            /*!< Scratch object table */
    uint8_t reserved[2];              /*!< 32 bits alignment */
};

/* Object table context */
static struct sst_obj_table_ctx_t sst_obj_table_ctx;

/* Object table size */
#define SST_OBJ_TABLE_SIZE            sizeof(struct sst_obj_table_t)

/* Object table entry size */
#define SST_OBJECTS_TABLE_ENTRY_SIZE  sizeof(struct sst_obj_table_entry_t)

/* Size of object table without any entries in it */
#define SST_EMPTY_OBJ_TABLE_SIZE  \
          (SST_OBJ_TABLE_SIZE - (SST_NUM_ASSETS * SST_OBJECTS_TABLE_ENTRY_SIZE))

/* Size of the data that is not required to authenticate */
#define SST_NON_AUTH_OBJ_TABLE_SIZE   sizeof(union sst_crypto_t)

/* Start position to store the object table data in the FS object */
#define SST_OBJECT_TABLE_OBJECT_OFFSET 0

/* Defines an object table with empty content */
#define SST_OBJECT_TABLE_EMPTY_SIZE  0
#define SST_OBJECT_TABLE_EMPTY       NULL

/* Invalid object table data value */
#define SST_OBJ_TABLE_INVALID 1

/* The associated data is the header minus the crypto data */
#define SST_CRYPTO_ASSOCIATED_DATA(crypto) ((uint8_t *)crypto + \
                                             SST_NON_AUTH_OBJ_TABLE_SIZE)

/* The associated data is the header, minus the the tag data */
#define SST_CRYPTO_ASSOCIATED_DATA_LEN (SST_OBJ_TABLE_SIZE - \
                                        SST_NON_AUTH_OBJ_TABLE_SIZE)

/* The sst_object_table_init function uses the static memory allocated for
 * the object data manipulation, in sst_object_table.c (g_sst_object), to load a
 * temporary object table to be validated at that stage.
 * To make sure the object table data fits in the static memory allocated for
 * object manipulation, the following macro checks if the memory allocated is
 * big enough, at compile time
 */

/* Check at compilation time if metadata fits in g_sst_object.data */
SST_UTILS_BOUND_CHECK(OBJ_TABLE_NOT_FIT_IN_STATIC_OBJ_DATA_BUF,
                      SST_OBJ_TABLE_SIZE, SST_MAX_ASSET_SIZE);

/*!
 * \struct sst_obj_table_ctx_t
 *
 * \brief Object table init context structure.
 */
struct sst_obj_table_init_ctx_t {
    struct sst_obj_table_t *p_table[SST_NUM_OBJ_TABLES]; /*!< Pointer to
                                                          *   object tables
                                                          */
    uint8_t invalid_table[SST_NUM_OBJ_TABLES];  /*!< Array to indicate if the
                                                 *   object table X has valid
                                                 *   data
                                                 */
};

/**
 * \brief Reads object table from persistent memory.
 *
 * \param[out] init_ctx  Pointer to the init object table context
 *
 */
__attribute__ ((always_inline))
__STATIC_INLINE void sst_object_table_fs_read_table(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    enum psa_sst_err_t err;
    /* FIXME: Read table from a persistent memory (flash location or FS) */

    /* Read file with the table 0 data */
    err = sst_flash_fs_file_read(SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_0),
                             SST_OBJ_TABLE_SIZE,
                             SST_OBJECT_TABLE_OBJECT_OFFSET,
                             (uint8_t *)init_ctx->p_table[SST_OBJ_TABLE_IDX_0]);
    if (err != PSA_SST_ERR_SUCCESS) {
        init_ctx->invalid_table[SST_OBJ_TABLE_IDX_0] = SST_OBJ_TABLE_INVALID;
    }

    /* Read file with the table 1 data */
    err = sst_flash_fs_file_read(SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_1),
                             SST_OBJ_TABLE_SIZE,
                             SST_OBJECT_TABLE_OBJECT_OFFSET,
                             (uint8_t *)init_ctx->p_table[SST_OBJ_TABLE_IDX_1]);
    if (err != PSA_SST_ERR_SUCCESS) {
        init_ctx->invalid_table[SST_OBJ_TABLE_IDX_1] = SST_OBJ_TABLE_INVALID;
    }
}

/**
 * \brief Writes object table in persistent memory.
 *
 * \param[in/out] obj_table  Pointer to the object table to generate
 *                           authentication
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
__attribute__ ((always_inline))
__STATIC_INLINE enum psa_sst_err_t sst_object_table_fs_write_table(
                                              struct sst_obj_table_t *obj_table)
{
    enum psa_sst_err_t err;
    uint32_t obj_table_id = SST_TABLE_FS_ID(sst_obj_table_ctx.scratch_table);
    uint8_t swap_table_idxs = sst_obj_table_ctx.scratch_table;


    /* Create file to store object table in the FS */
    err = sst_flash_fs_file_create(obj_table_id,
                                   SST_OBJ_TABLE_SIZE,
                                   SST_OBJ_TABLE_SIZE,
                                   (const uint8_t *)obj_table);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Swap active and scratch table values */
    sst_obj_table_ctx.scratch_table = sst_obj_table_ctx.active_table;
    sst_obj_table_ctx.active_table = swap_table_idxs;

    return PSA_SST_ERR_SUCCESS;
}

#ifdef SST_ENCRYPTION
/**
 * \brief Sets crypto key for object table.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_object_table_set_crypto_key(void)
{
    enum psa_sst_err_t err;
    uint8_t sst_key[SST_KEY_LEN_BYTES];   /*!< Secure storage system key */

    err = sst_crypto_getkey(sst_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_crypto_setkey(sst_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    return err;
}

/**
 * \brief Generates table authentication
 *
 * \param[in/out] obj_table  Pointer to the object table to generate
 *                           authentication
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_object_table_generate_auth_tag(
                                              struct sst_obj_table_t *obj_table)
{
    union sst_crypto_t *crypto = &obj_table->crypto;

    /* Get new IV */
    sst_crypto_get_iv(crypto);

    return sst_crypto_generate_auth_tag(crypto,
                                        SST_CRYPTO_ASSOCIATED_DATA(crypto),
                                        SST_CRYPTO_ASSOCIATED_DATA_LEN);
}

/**
 * \brief Authenticates tables of objects.
 *
 * \param[in/out] init_ctx  Pointer to the object table to authenticate
 *
 */
__attribute__ ((always_inline))
__STATIC_INLINE void sst_object_tables_authenticate(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    enum psa_sst_err_t err;
    union sst_crypto_t *crypto =
                                &init_ctx->p_table[SST_OBJ_TABLE_IDX_0]->crypto;

    /* Authenticate table 0 if data is valid */
    if (init_ctx->invalid_table[SST_OBJ_TABLE_IDX_0] != SST_OBJ_TABLE_INVALID) {
        err = sst_crypto_authenticate(crypto,
                                      SST_CRYPTO_ASSOCIATED_DATA(crypto),
                                      SST_CRYPTO_ASSOCIATED_DATA_LEN);
        if (err != PSA_SST_ERR_SUCCESS) {
            init_ctx->invalid_table[SST_OBJ_TABLE_IDX_0] =
                                                          SST_OBJ_TABLE_INVALID;
        }
    }

    /* Authenticate table 1 if data is valid */
    if (init_ctx->invalid_table[SST_OBJ_TABLE_IDX_1] != SST_OBJ_TABLE_INVALID) {
        crypto = &init_ctx->p_table[SST_OBJ_TABLE_IDX_1]->crypto;

        err = sst_crypto_authenticate(crypto,
                                      SST_CRYPTO_ASSOCIATED_DATA(crypto),
                                      SST_CRYPTO_ASSOCIATED_DATA_LEN);
        if (err != PSA_SST_ERR_SUCCESS) {
            init_ctx->invalid_table[SST_OBJ_TABLE_IDX_1] =
                                                          SST_OBJ_TABLE_INVALID;
        }
    }
}
#endif

/**
 * \brief Saves object table in the persistent memory.
 *
 * \param[in/out] obj_table  Pointer to the object table to save
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_object_table_save_table(
                                              struct sst_obj_table_t *obj_table)
{
#ifdef SST_ENCRYPTION
    enum psa_sst_err_t err;
#endif

    obj_table->swap_count++;

    if (obj_table->swap_count == SST_FLASH_DEFAULT_VAL) {
        /* When a flash block is erased, the default value is usually 0xFF
         * (i.e. all 1s). Since the swap count is updated last (when encryption
         * is disabled), it is possible that due to a power failure, the swap
         * count value in metadata header is 0xFFFF..., which mean it will
         * appear to be most recent block.
         */
        obj_table->swap_count = 0;
    }

#ifdef SST_ENCRYPTION
    /* Set object table key */
    sst_object_table_set_crypto_key();

    /* Generate authentication tag from the current table content */
    err = sst_object_table_generate_auth_tag(obj_table);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }
#endif /* SST_ENCRYPTION */

    return sst_object_table_fs_write_table(obj_table);
}

/**
 * \brief Checks the validity of the table version.
 *
 * \param[in/out] init_ctx  Pointer to the init object table context
 *
 */
static void sst_object_table_validate_version(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    /* Looks for exact version number.
     * FIXME: backward compatibility could be considered in future revisions.
     */
    if (SST_OBJECT_SYSTEM_VERSION !=
        init_ctx->p_table[SST_OBJ_TABLE_IDX_0]->version) {
        init_ctx->invalid_table[SST_OBJ_TABLE_IDX_0] = SST_OBJ_TABLE_INVALID;
    }

    if (SST_OBJECT_SYSTEM_VERSION !=
        init_ctx->p_table[SST_OBJ_TABLE_IDX_1]->version) {
        init_ctx->invalid_table[SST_OBJ_TABLE_IDX_1] = SST_OBJ_TABLE_INVALID;
    }
}

/**
 * \brief Sets the active object table based on the swap count and validity of
 *        the object table data.
 *
 * \param[in] init_ctx Pointer to the init object table context
 *
 */
static enum psa_sst_err_t sst_set_active_object_table(
                                const struct sst_obj_table_init_ctx_t *init_ctx)
{

    uint8_t table0_swap_count =
                             init_ctx->p_table[SST_OBJ_TABLE_IDX_0]->swap_count;
    uint8_t table1_swap_count =
                             init_ctx->p_table[SST_OBJ_TABLE_IDX_1]->swap_count;

    /* Check if there is an invalid object table */

    if ((init_ctx->invalid_table[SST_OBJ_TABLE_IDX_0] == SST_OBJ_TABLE_INVALID)
         && (init_ctx->invalid_table[SST_OBJ_TABLE_IDX_1] ==
                                                       SST_OBJ_TABLE_INVALID)) {
        /* Both tables are invalid */
        return PSA_SST_ERR_SYSTEM_ERROR;
    } else if (init_ctx->invalid_table[SST_OBJ_TABLE_IDX_0] ==
                                                        SST_OBJ_TABLE_INVALID) {
          /* Table 0 is invalid, the the active one is table 1 */
          sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
          sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

          return PSA_SST_ERR_SUCCESS;
    } else {
        /* Table 1 is invalid, the the active one is table 0 */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;

        return PSA_SST_ERR_SUCCESS;
    }

    /* Logic: if the swap count is 0, then it has rolled over. The object table
     * with a swap count of 0 is the latest one, unless the other block has a
     * swap count of 1, in which case the roll over occurred in the previous
     * update. In all other cases, the table with the highest swap count is the
     * latest one.
     */
    if ((table1_swap_count == 0) && (table0_swap_count != 1)) {
        /* Table 1 swap count has rolled over and table 0 swap count has not,
         * so table 1 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

    } else if ((table0_swap_count == 0) && (table1_swap_count != 1)) {
        /* Table 0 swap count has rolled over and table 1 swap count has not,
         * so table 0 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;

    } else if (table1_swap_count > table0_swap_count) {
        /* Neither swap count has just rolled over and table 1 has a
         * higher swap count, so table 1 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

    } else {
        /* Neither swap count has just rolled over and table 0 has a
         * higher or equal swap count, so table 0 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;
    }


    /* If active object table is table 1, then copy the content into the
     * SST object table context.
     */
    if (sst_obj_table_ctx.active_table == SST_OBJ_TABLE_IDX_1) {
        sst_utils_memcpy(&sst_obj_table_ctx.obj_table,
                         init_ctx->p_table[SST_OBJ_TABLE_IDX_1],
                         SST_OBJ_TABLE_SIZE);
    }

    return PSA_SST_ERR_SUCCESS;
}

/**
 * \brief Gets table's entry index based on the given object UUID.
 *
 * \param[in]  uuid  Object UUID
 * \param[out] idx   Pointer to store the entry's index
 *
 * \return Returns PSA_SST_ERR_SUCCESS and index of the table, if object exists
 *         in the table. Otherwise, it returns PSA_SST_ERR_ASSET_NOT_FOUND.
 */
static enum psa_sst_err_t sst_get_object_entry_idx(uint32_t uuid, uint32_t *idx)
{
    uint32_t i;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    for (i = 0; i < SST_OBJ_TABLE_ENTRIES; i++) {
        if (p_table->obj_db[i].uuid == uuid) {
            *idx = i;
            return PSA_SST_ERR_SUCCESS;
        }
    }

    return PSA_SST_ERR_ASSET_NOT_FOUND;
}

/**
 * \brief Gets free index in the table
 *
 * \note The table is dimensioned to fit all the object defined in asset_defs.h
 *       plus an extra one.
 *
 * \return Returns free index in the table
 */
static uint32_t sst_table_free_idx(void)
{
    uint32_t idx;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    for (idx = 0; idx < SST_OBJ_TABLE_ENTRIES; idx++) {
        if (p_table->obj_db[idx].uuid == SST_INVALID_UUID) {
            break;
        }
    }

    return idx;
}

/**
 * \brief Deletes an entry from the table
 *
 * \param[in] idx  Entry index to delete
 *
 */
static void sst_table_delete_entry(uint32_t idx)
{
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    p_table->obj_db[idx].uuid = SST_INVALID_UUID;

#ifdef SST_ENCRYPTION
    sst_utils_memset(p_table->obj_db[idx].tag, SST_DEFAULT_EMPTY_BUFF_VAL,
                     SST_OBJECTS_TABLE_ENTRY_SIZE);
#else
    p_table->obj_db[idx].version = 0;
#endif
}

enum psa_sst_err_t sst_object_table_create(void)
{
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    /* Initialize object structure */
    sst_utils_memset(&sst_obj_table_ctx, SST_DEFAULT_EMPTY_BUFF_VAL,
                     sizeof(struct sst_obj_table_ctx_t));

    /* Invert the other in the context as sst_object_table_save_table will
     * use the scratch index to create and store the current table.
     */
    sst_obj_table_ctx.active_table = SST_OBJ_TABLE_IDX_1;
    sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

    p_table->version = SST_OBJECT_SYSTEM_VERSION;

    /* Save object table contents */
    return sst_object_table_save_table(p_table);
}

enum psa_sst_err_t sst_object_table_init(uint8_t *obj_data)
{
    enum psa_sst_err_t err;
    struct sst_obj_table_init_ctx_t init_ctx = {
        .p_table = {&sst_obj_table_ctx.obj_table, 0},
        .invalid_table = {0, 0}
    };

    init_ctx.p_table[SST_OBJ_TABLE_IDX_1] = (struct sst_obj_table_t *)obj_data;

    /* Read table from the file system */
    sst_object_table_fs_read_table(&init_ctx);

#ifdef SST_ENCRYPTION
    /* Set object table key */
    sst_object_table_set_crypto_key();

    /* Authenticate table */
    sst_object_tables_authenticate(&init_ctx);
#endif /* SST_ENCRYPTION */

    /* Check tables version */
    sst_object_table_validate_version(&init_ctx);

    /* Set active tables */
    err = sst_set_active_object_table(&init_ctx);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Remove the old object table file */
    err = sst_flash_fs_file_delete(SST_TABLE_FS_ID(
                                              sst_obj_table_ctx.scratch_table));
    if (err != PSA_SST_ERR_SUCCESS && err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        return err;
    }

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_object_table_obj_exist(uint32_t uuid)
{
    uint32_t idx = 0;

    return sst_get_object_entry_idx(uuid, &idx);
}

enum psa_sst_err_t sst_object_table_get_free_fid(uint32_t *p_fid)
{
    uint32_t fid;
    uint32_t idx;

    idx = sst_table_free_idx();

    /* There first two file IDs are reserved for the active table
     * and scratch table files.
     */
    fid = SST_OBJECT_FS_ID(idx);

    /* Check if there is a file in the persistent area with that ID. That
     * can happened when the system is rebooted (e.g. power cut, ...) in the
     * middle of a create, write or delete operation.
     */
    if (sst_flash_fs_file_exist(fid) == PSA_SST_ERR_SUCCESS) {
        /* Remove old file from the persistent area, to keep it consistent
         * with the table content.
         */
        return sst_flash_fs_file_delete(fid);

    }

    *p_fid = fid;

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_object_table_set_obj_tbl_info(uint32_t uuid,
                          const struct sst_obj_table_info_t *obj_tbl_info)
{
    enum psa_sst_err_t err;
    uint32_t idx = 0;
    uint32_t backup_idx = 0;
    struct sst_obj_table_entry_t backup_entry;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    err = sst_get_object_entry_idx(uuid, &backup_idx);
    if (err == PSA_SST_ERR_SUCCESS) {
        /* If an entry exists for this UUID, it creates a backup copy in case
         * an error happens while updating the new table in the filesystem.
         */
        sst_utils_memcpy((uint8_t *)&backup_entry,
                         (const uint8_t *)&p_table->obj_db[backup_idx],
                         SST_OBJECTS_TABLE_ENTRY_SIZE);

        /* Deletes old object information if it exist in the table */
        sst_table_delete_entry(backup_idx);
    }

    idx = SST_OBJECT_FS_ID_TO_IDX(obj_tbl_info->fid);
    p_table->obj_db[idx].uuid = uuid;

    /* Add new object information */
#ifdef SST_ENCRYPTION
    sst_utils_memcpy(p_table->obj_db[idx].tag, obj_tbl_info->tag,
                     SST_TAG_LEN_BYTES);
#else
    p_table->obj_db[idx].version = obj_tbl_info->version;
#endif

    err = sst_object_table_save_table(p_table);
    if (err != PSA_SST_ERR_SUCCESS) {
        /* Rollback the change in the table */
        sst_utils_memcpy((uint8_t *)&p_table->obj_db[backup_idx],
                        (const uint8_t *)&backup_entry,
                        SST_OBJECTS_TABLE_ENTRY_SIZE);

        sst_table_delete_entry(idx);
    }

    return err;
}

enum psa_sst_err_t sst_object_table_get_obj_tbl_info(uint32_t uuid,
                                      struct sst_obj_table_info_t *obj_tbl_info)
{
    enum psa_sst_err_t err;
    uint32_t idx;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    err = sst_get_object_entry_idx(uuid, &idx);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    obj_tbl_info->fid = SST_OBJECT_FS_ID(idx);

#ifdef SST_ENCRYPTION
    sst_utils_memcpy(obj_tbl_info->tag, p_table->obj_db[idx].tag,
                     SST_TAG_LEN_BYTES);
#else
    obj_tbl_info->version = p_table->obj_db[idx].version;
#endif

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_object_table_delete_object(uint32_t uuid)
{
    uint32_t backup_idx = 0;
    struct sst_obj_table_entry_t backup_entry;
    enum psa_sst_err_t err;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    /* Create a backup copy in case an error happens while updating the new
     * table in the filesystem.
     */
    err = sst_get_object_entry_idx(uuid, &backup_idx);
    if (err != PSA_SST_ERR_SUCCESS) {
        /* If the object is not present in the table, it returns an error
         * to not generate a new file where the table content is the same.
         * Otherwise, that could be used by an attacker to get the encryption
         * key.
         */
        return err;
    }

    sst_utils_memcpy((uint8_t *)&backup_entry,
                     (const uint8_t *)&p_table->obj_db[backup_idx],
                     SST_OBJECTS_TABLE_ENTRY_SIZE);

    sst_table_delete_entry(backup_idx);

    err = sst_object_table_save_table(p_table);
    if (err != PSA_SST_ERR_SUCCESS) {
       /* Rollback the change in the table */
       sst_utils_memcpy((uint8_t *)&p_table->obj_db[backup_idx],
                        (const uint8_t *)&backup_entry,
                        SST_OBJECTS_TABLE_ENTRY_SIZE);
    }

    return err;
}

enum psa_sst_err_t sst_object_table_delete_old_table(void)
{
    uint32_t table_id = SST_TABLE_FS_ID(sst_obj_table_ctx.scratch_table);

    return sst_flash_fs_file_delete(table_id);
}
