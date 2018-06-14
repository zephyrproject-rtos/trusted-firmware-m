/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SST_ASSET_MACROS_H__
#define __TFM_SST_ASSET_MACROS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_sst_asset_defs.h"

/*!
 * \def TFM_SST_ASSET_IS_ASSET_CAT(sst_def_cat_type, type)
 *
 * \brief Macro to check if the asset category type (type) is equal to the
 *        TF-M SST asset defined category types (sst_def_cat_type).
 *
 * \param[in] sst_def_cat_type  TF-M SST asset defined category type
 * \param[in] type              Asset type
 *
 * \return Returns 1 if the asset type is equal to the TF-M SST asset defined
 *         type. Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_IS_ASSET_CAT(sst_def_cat_type, type) \
        (((type & TFM_SST_ASSET_CAT_VENDOR_DEFINED) != 0) && \
          ((type & TFM_SST_ASSET_CAT_TYPE_MASK) == sst_def_cat_type))

/*!
 * \def TFM_SST_ASSET_IS_ASSET_TYPE(sst_def_type, type)
 *
 * \brief Macro to check if the asset type (type) is equal to the
 *        TF-M SST asset defined type (sst_def_type).
 *
 * \param[in] sst_def_type  TF-M SST asset defined type
 * \param[in] type          Asset type
 *
 * \return Returns 1 if the asset type is equal to the TF-M SST asset defined
 *         type. Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_IS_ASSET_TYPE(sst_def_type, type) \
        (((type & TFM_SST_ASSET_CAT_VENDOR_DEFINED) != 0) && \
          (type == sst_def_type))

/*!
 * \def TFM_SST_ASSET_IS_VENDOR_DEFINED(type)
 *
 * \brief Macro to check if the asset type is vendor defined.
 *
 * \param[in] type  Asset type
 *
 * \return Returns 1 if the asset type is equal to TFM_SST_ASSET_VENDOR_DEFINED.
 *         Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_IS_VENDOR_DEFINED(type) \
        ((type & TFM_SST_ASSET_VENDOR_DEFINED) != 0)

/*!
 * \def TFM_SST_ASSET_IS_KEY_TYPE(type)
 *
 * \brief Macro to check if the asset type is a key.
 *
 * \param[in] type  Asset type
 *
 * \return Returns 1 if it is a key. Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_IS_KEY_TYPE(type) \
        ((type & TFM_SST_ASSET_KEY_MASK) != 0)

/*!
 * \def TFM_SST_ASSET_IS_ASYMMETRIC_KEY_TYPE(type)
 *
 * \brief Macro to check if the asset type is a symmetric key.
 *
 * \param[in] type  Asset type
 *
 * \return Returns 1 if it is an asymmetric key. Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_IS_ASYMMETRIC_KEY_TYPE(type) \
        ((type & TFM_SST_ASSET_KEY_ASYMMETRIC_MASK) != 0)

/*!
 * \def TFM_SST_ASSET_HAS_ATTR(sst_def_asset_attr, attrs)
 *
 * \brief Macro to check if the asset attributes (attrs) have a specific TF-M
 *        SST asset defined bit attribute (sst_def_asset_attr).
 *
 * \param[in] sst_def_type  TF-M SST asset defined bit attribute
 * \param[in] attrs         Asset attributes
 *
 * \return Returns 1 if the asset has the TF-M SST asset defined
 *         attribute. Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_HAS_ATTR(sst_def_asset_attr, attrs) \
        (((attrs & sst_def_asset_attr) != 0) && \
         ((attrs & TFM_SST_ASSET_ATTR_VENDOR_DEFINED) == 0))

/*!
 * \def TFM_SST_ASSET_HAS_ATTR_VENDOR_DEFINED(attrs)
 *
 * \brief Macro to check if the asset attributes are vendor defined.
 *
 * \param[in] attrs  Asset attributes
 *
 * \return Returns 1 if the asset type is equal to TFM_SST_ASSET_VENDOR_DEFINED.
 *         Otherwise, it returns 0.
 */
#define TFM_SST_ASSET_HAS_ATTR_VENDOR_DEFINED(attrs) \
        ((attrs & TFM_SST_ASSET_ATTR_VENDOR_DEFINED) != 0)

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SST_ASSET_MACROS_H__ */
