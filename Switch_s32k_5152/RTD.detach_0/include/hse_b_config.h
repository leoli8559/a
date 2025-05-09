/**
*   @file    hse_b_config.h
*
*   @brief   HSE Basic features.
*   @details This file contains the HSE Basic features.
*
*   @addtogroup hse_b_config HSE Basic Features
*   @ingroup class_features_impl
*   @{
*/
/*==================================================================================================
*
*    Copyright 2019 - 2022 NXP.
*
*   This software is owned or controlled by NXP and may only be used strictly in accordance with
*   the applicable license terms. By expressly accepting such terms or by downloading, installing,
*   activating and/or otherwise using the software, you are agreeing that you have read, and that
*   you agree to comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain, install, activate or
*   otherwise use the software.
==================================================================================================*/
/*==================================================================================================
==================================================================================================*/

#ifndef HSE_B_CONFIG_H
#define HSE_B_CONFIG_H


#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
/**
* @file           hse_b_config.h
*/

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

    /**< \defdisable{#define,HSE_SPT_FLASHLESS_DEV}*/   /**< @brief  The service is flashless (external flash). */
    #define HSE_SPT_INTERNAL_FLASH_DEV                  /**< @brief  Device has internal flash. */
    /*-------------------------------------------------------*/
    #define HSE_SPT_RANDOM          /**< @brief Support for Random Number Generation. */
    /*-------------------------------------------------------*/
    #define HSE_SPT_SHE             /**< @brief Support for SHE specification.
                                                @note AES and CMAC features must be enabled. */
    /*-------------------------------------------------------*/
    #define HSE_SPT_AES             /**< @brief Support for AES_(128, 192, 256)_(ECB, CBC, CFB, OFB, CTR). AES-CBC is supported on all platforms by default. */

    #ifdef HSE_SPT_AES
    #define HSE_SPT_CIPHER_BLOCK_MODE_CFB       /**< @brief  AES-CFB cipher mode supported */
    #define HSE_SPT_CIPHER_BLOCK_MODE_CTR       /**< @brief  AES-CTR cipher mode supported */
    #define HSE_SPT_CIPHER_BLOCK_MODE_ECB       /**< @brief  AES-ECB cipher mode supported */
    #define HSE_SPT_CIPHER_BLOCK_MODE_OFB       /**< @brief  AES-OFB cipher mode supported */
    #endif /*HSE_SPT_AES*/

    /**< \defdisable{#define,HSE_SPT_XTS_AES}*/   /**< @brief Support for XTS AES  */
    #define HSE_SPT_AEAD_GCM                      /**< @brief Support for AEAD AES GCM as defined  in FIPS PUB 197, NIST SP 800-38D, RFC-5288 and RFC-4106. */
    #define HSE_SPT_AEAD_CCM                      /**< @brief Support for AEAD AES CCM as defined  in FIPS PUB 197, NIST SP 800-38C, RFC-6655 and RFC-4309. */
    /**< \defdisable{#define,HSE_SPT_AUTHENC}*/   /**< @brief Support for Dual Purpose Crypto Service (Authenticated encryption) */
    /**< \defdisable{#define,HSE_SPT_CRC32}*/     /**< @brief Support CRC computation */

    /*-------------------------------------------------------*/
    #define HSE_SPT_HASH                      /**< @brief Hash support */
#ifdef HSE_SPT_HASH
    #define HSE_SPT_SHA1                      /**< @brief Support for SHA-1 as defined  in FIPS PUB 180-4. */
    #define HSE_SPT_SHA2_224                  /**< @brief Support for SHA2_224 in FIPS PUB 180-4. */
    #define HSE_SPT_SHA2_256                  /**< @brief Support for SHA2_256 in FIPS PUB 180-4. */
    #define HSE_SPT_SHA2_384                  /**< @brief Support for SHA2_384 in FIPS PUB 180-4. Scatter gather feature is not supported. */
    #define HSE_SPT_SHA2_512                  /**< @brief Support for SHA2_512 in FIPS PUB 180-4. Scatter gather feature is not supported. */
    #define HSE_SPT_SHA2_512_224              /**< @brief Support for SHA2_512_224 in FIPS PUB 180-4. Scatter gather feature is not supported. */
    #define HSE_SPT_SHA2_512_256              /**< @brief Support for SHA2_512_256 in FIPS PUB 180-4. Scatter gather feature is not supported. */
    /**< \defdisable{#define,HSE_SPT_SHA3}*/  /**< @brief Support for SHA3_(224, 256, 384, 512) as defined  in FIPS PUB 202. */
    #define HSE_SPT_MIYAGUCHI_PRENEEL         /**< @brief Miyaguchi-Preneel compression function (SHE spec support)*/
#endif /* ifdef HSE_SPT_HASH */

    /*-------------------------------------------------------*/
    #define HSE_SPT_MAC                       /**< @brief MAC support */
#ifdef HSE_SPT_MAC
    #define HSE_SPT_FAST_CMAC                     /**< @brief Support for AES fast CMAC (optimized) */
    #define HSE_SPT_CMAC                          /**< @brief Support for AES CMAC as defined  in NIST SP 800-38B. */
    #define HSE_SPT_HMAC                          /**< @brief Support for HMAC_SHA1 and HMAC_SHA2_(224, 256, 384, 512) as defined in FIPS PUB 198-1 and SP 800-107. */
    #define HSE_SPT_GMAC                          /**< @brief Support for AES GMAC as defined  in NIST SP 800-38D. */
    /**< \defdisable{#define,HSE_SPT_XCBC_MAC}*/  /**< @brief Support for AES XCBC_MAC_96 as defined  in RFC-3566. */
#endif /* ifdef HSE_SPT_MAC */

    /*-------------------------------------------------------*/
    #define HSE_SPT_CMAC_WITH_COUNTER             /**< @brief Support for CMAC with counter */

    /*-------------------------------------------------------*/
    /**< \defdisable{#define,HSE_SPT_SIPHASH}*/   /**< @brief Support for SipHash */

    /*-------------------------------------------------------*/
    #define HSE_SPT_RSA                 /**< @brief RSA support */
#ifdef HSE_SPT_RSA
    #define HSE_SPT_RSAES_NO_PADDING    /**< @brief  RSA modular exponentiation operations( RSAEP and RSADP). */
    #define HSE_SPT_RSAES_OAEP          /**< @brief Support for RSAES_OAEP as defined by RFC-8017. */
    #define HSE_SPT_RSAES_PKCS1_V15     /**< @brief Support for RSAES_PKCS1_V15 as defined  by PKCS#1 v2.2. */
    #define HSE_SPT_RSASSA_PSS          /**< @brief Support for RSASSA_PSS as defined  by FIPS 186-4. */
    #define HSE_SPT_RSASSA_PKCS1_V15    /**< @brief Support RSASSA_PKCS1_V15 as defined by PKCS#1 v2.2. */
#endif /* ifdef HSE_SPT_RSA */

    /*-------------------------------------------------------*/
    #define HSE_SPT_ECC                                   /**< @brief Support for ECC */
    #define HSE_SPT_CLASSIC_DH                            /**< @brief Support for generate key pair, DH share secret computation as defined in FIPS 186-4 */
#ifdef HSE_SPT_ECC
    #define HSE_SPT_ECDH                                  /**< @brief ECDH support */
    #define HSE_SPT_ECDSA                                 /**< @brief ECDSA support */
    #define HSE_SPT_EDDSA                                 /**< @brief Twisted Edwards EDDSA (e.g. ED25519) support */
    #define HSE_SPT_MONTDH                                /**< @brief Montgomery DH (e.g X25519 curve) support */
    #define HSE_SPT_ECC_USER_CURVES                       /**< @brief Support to set ECC curve (not supported by default)*/
    #define HSE_SPT_EC_SEC_SECP256R1                      /**< @brief Support Ecc p256v1 */
    #define HSE_SPT_EC_SEC_SECP384R1                      /**< @brief Support Ecc SECP p384r1 */
    #define HSE_SPT_EC_SEC_SECP521R1                      /**< @brief Support Ecc SECP p521r1 */
    #define HSE_SPT_EC_BRAINPOOL_BRAINPOOLP256R1          /**< @brief Support Ecc BrainPool p256r1 */
    #define HSE_SPT_EC_BRAINPOOL_BRAINPOOLP320R1          /**< @brief Support Ecc BrainPool p320r1 */
    #define HSE_SPT_EC_BRAINPOOL_BRAINPOOLP384R1          /**< @brief Support Ecc BrainPool p384r1 */
    #define HSE_SPT_EC_BRAINPOOL_BRAINPOOLP512R1          /**< @brief Support Ecc BrainPool p521r1 */
    #define HSE_SPT_EC_25519_ED25519                      /**< @brief Twisted Edwards ED25519 curve support (used with EdDSA )*/
    /**< \defdisable{#define,HSE_SPT_EC_448_ED448} */     /**< @brief Twisted Edwards ED448 curve support (used with EdDSA )*/
    #define HSE_SPT_EC_25519_CURVE25519                   /**< @brief Montgomery X25519 curve support (used with MONTDH) */
    /**< \defdisable{#define,HSE_SPT_EC_448_CURVE448}*/   /**< @brief Montgomery X448 curve support (used with MONTDH) */
    #define HSE_SPT_BURMESTER_DESMEDT                     /**< @brief Burmester-Desmedt Protocol support */
    #define HSE_SPT_ECC_COMPRESSED_KEYS                   /**< @brief Support Ecc Weierstrass curve compressed keys */
#endif /*ifdef HSE_SPT_ECC */

    /*-------------------------------------------------------*/
    #define HSE_SPT_KEY_GEN             /**< @brief Key Generate support */
#ifdef HSE_SPT_KEY_GEN
    #define HSE_SPT_SYM_RND_KEY_GEN     /**< @brief Support for symmetric random key generation. */

    #ifdef HSE_SPT_ECC
    #define HSE_SPT_ECC_KEY_PAIR_GEN    /**< @brief Support for ECC key-pair generation. */
    #endif /* HSE_SPT_ECC */

    #ifdef HSE_SPT_RSA
    #define HSE_SPT_RSA_KEY_PAIR_GEN                                /**< @brief Support for RSA key-pair generation. */
    #define HSE_SPT_TLS12_RSA_PRE_MASTER_SECRET_GEN                 /**< @brief Support for RSA key exchange. */
    #endif /* HSE_SPT_RSA */

    #ifdef HSE_SPT_CLASSIC_DH
    #define HSE_SPT_CLASSIC_DH_KEY_PAIR_GEN                         /**< @brief Support for Classic DH key-pair generation. */
    #endif /* HSE_SPT_CLASSIC_DH */
#endif

    /*-------------------------------------------------------*/
    #define HSE_SPT_KEY_DERIVE              /**< @brief KDF support */
#ifdef HSE_SPT_KEY_DERIVE
    #define HSE_SPT_KDF_NXP_GENERIC         /**< @brief NXP Generic KDF. */
    #define HSE_SPT_KDF_SP800_56C_ONESTEP   /**< @brief Support for KDF One-step as defined  by SP800-56C rev1. HMAC and XCBC_MAC options as PRF algorithm are not supported */
    #define HSE_SPT_KDF_SP800_56C_TWOSTEP   /**< @brief Support for KDF Two-step as defined  by SP800-56C rev1. HMAC and XCBC_MAC options as PRF algorithm are not supported */
    #define HSE_SPT_KDF_SP800_108           /**< @brief Support for KDF(Counter, Feedback, Pipeline) as defined  by SP800-108. Only counter mode is supported. HMAC and XCBC_MAC options as PRF algorithm are not supported */
    #define HSE_SPT_KDF_ANS_X963            /**< @brief Support for KDF as defined  by ANS X9.63. HMAC and XCBC_MAC options as PRF algorithm are not supported */
    #define HSE_SPT_KDF_ISO18033_KDF1       /**< @brief Support for KDF1 as defined by ISO18033. HMAC and XCBC_MAC options as PRF algorithm are not supported */
    #define HSE_SPT_KDF_ISO18033_KDF2       /**< @brief Support for KDF2 as defined by ISO18033. HMAC and XCBC_MAC options as PRF algorithm are not supported */
    #define HSE_SPT_PBKDF2                  /**< @brief Support for PBKDF2 as defined as defined by PKCS#5 v2.1 and RFC-8018. Support of HMAC is provided with Hash algos SHA-1, SHA-224, and SHA-256. */
    #define HSE_SPT_KDF_TLS12_PRF           /**< @brief KDF Support for TLS 1.2 as defined by RFC-5246. Support of HMAC is provided with Hash algos SHA-1, SHA-224, and SHA-256. */
    #define HSE_SPT_HKDF                    /**< @brief Support for HMAC-based Extract-and-Expand KDF as defined by RFC-5869. Support of HMAC is provided with Hash algos SHA-1, SHA-224, and SHA-256. */
    /**< \defdisable{#define,HSE_SPT_KDF_IKEV2}*/              /**< @brief KDF Support for IKEv2 as defined  by RFC-4306. */
#endif /* ifdef HSE_SPT_KEY_DERIVE */

    /*-------------------------------------------------------*/
    #define HSE_SPT_NXP_ROM_KEYS                /**< @brief Support NXP ROM keys. */
#ifdef HSE_SPT_NXP_ROM_KEYS
    #define HSE_SPT_NXP_ROM_PUB_KEYS            /**< @brief Support NXP ROM public keys. */
#endif

    #define HSE_SPT_FORMAT_KEY_CATALOGS     /**< @brief Support Format Key Catalogs service */
    #define HSE_SPT_GET_KEY_INFO            /**< @brief Support Get Key Info Service. */
    #define HSE_SPT_KEY_VERIFY              /**< @brief Support Key Verify Service. */
    #define HSE_SPT_IMPORT_KEY              /**< @brief Support Import Key Service. */
    #define HSE_SPT_EXPORT_KEY              /**< @brief Support Export Key Service. */
    #define HSE_SPT_KEY_MGMT_POLICIES       /**< @brief Support Key Management configurable policies. */

    /*-------------------------------------------------------*/
    #define HSE_SPT_PUBLISH_NVM_KEYSTORE_RAM_TO_FLASH                /**< @brief Support Publishing the keystore from RAM to data flash */
    
    /*-------------------------------------------------------*/
    #define HSE_SPT_MONOTONIC_COUNTERS            /**< @brief Monotonic Counter support */
#ifdef HSE_SPT_MONOTONIC_COUNTERS
    #define HSE_NUM_OF_MONOTONIC_COUNTERS            (16U) /**< @brief   The supported number of monotonic counters */
    #define HSE_NVM_SPT_MONOTONIC_CNT_NO_OF_SECTORS  (1U)  /**< @brief The supported number of passive sectors used by monotonic counters */
#endif
    #define HSE_SPT_NXP_KEY_STORE_NO_OF_SECTORS      (1U)    /**< @brief The supported number of active sectors used by NXP key store */
    #define HSE_SECTOR_SIZE                          (8192U) /**< @brief The size of Flash Sector in S32K3xx Devices */
    /*-------------------------------------------------------*/
    #define HSE_SPT_BOOTDATASIGN                   /**< @brief Boot Data Sign Support  */
#ifdef HSE_SPT_BOOTDATASIGN
    #define HSE_SPT_BSB                            /**< @brief Basic Secure Booting(ASB) Support  */
#endif

    /*-------------------------------------------------------*/

    #define HSE_SPT_SMR_CR                           /**< @brief Advance Secure Booting(ASB) Secure memory regions verification (SMR) & Core Reset(CR) Table Support */
#ifdef HSE_SPT_SMR_CR
    #define HSE_NUM_OF_SMR_ENTRIES          (8U)                  /**< @brief   The supported number of SMR entries*/
    #define HSE_NUM_OF_CORE_RESET_ENTRIES   (3U)                  /**< @brief   The supported number of CORE RESET entries*/
    #define HSE_SPT_SMR_DECRYPT                                   /**< @brief   Support encrypted SMRs. */
#endif
    /*-------------------------------------------------------*/
    #define HSE_SPT_STREAM_CTX_IMPORT_EXPORT      /**< @brief Support Import/Export of streaming context for symmetric operations */


    #define HSE_SPT_MU_CONFIG                     /**< @brief Support MU configuration */

    #define HSE_SPT_CUST_SEC_POLICY               /**< @brief Support of Customer Security Policy */
    #define HSE_SPT_OEM_SEC_POLICY                /**< @brief Support of Oem Security Policy */

    #define HSE_SPT_PHYSICAL_TAMPER_CONFIG                          /**< @brief Support of active tamper */
    #define HSE_NUM_OF_PHYSICAL_TAMPER_INSTANCES         (1U)       /**< @brief  Number of Physical Tamper Instances */


    /**< \defdisable{#define,HSE_SPT_SELF_TEST}*/  /**< @brief Support self test */

    #define HSE_SPT_MEM_REGION_PROTECT             /**< @brief Support memory region protection */

    #define HSE_SPT_OTA_FIRMWARE_UPDATE            /**< @brief Support OTA Firmware Update */
    #define HSE_SPT_OTA_SBAF_UPDATE                /**< @brief Support SBAF update */
    #define HSE_SPT_FW_BACKUP_ENABLE               /**< @brief Support BACKUP Feature. */
    #define HSE_SPT_FW_INTEGRITY_CHECK             /**< @brief Support HSE flash memory integrity check. */

    #define HSE_SPT_SGT_OPTION                     /**< @brief Enable support for Scatter Gatter Table */
#ifdef HSE_SPT_SGT_OPTION
    #define HSE_MAX_NUM_OF_SGT_ENTRIES  (8U)        /**< @brief Maximum number for SGT entries */
#endif

    /*-------------------------------------------------------*/
    #define HSE_NUM_OF_MU_INSTANCES       (2U)      /**< @brief  The maxim number of MU interfaces */
    #define HSE_NUM_OF_CHANNELS_PER_MU    (4U)      /**< @brief  The maxim number of channels per MU interface  */
    #define HSE_STREAM_COUNT              (2U)      /**< @brief  HSE stream count per interface */

#ifdef HSE_SPT_ECC_USER_CURVES
    #define HSE_NUM_OF_USER_ECC_CURVES    (1U)      /**< @brief The number of ECC curves the user can load into the HSE */
#endif

    #define HSE_TOTAL_NUM_OF_KEY_GROUPS   (32U)     /**< @brief  The total number of catalog configuration entries for both NVM and RAM catalogs.*/
    #define HSE_MAX_RAM_STORE_SIZE        (6144U)   /**< @brief  RAM key store size (in bytes) */
    #define HSE_MAX_NVM_STORE_SIZE        (7768U + ((HSE_SPT_NXP_KEY_STORE_NO_OF_SECTORS - 1U) * HSE_SECTOR_SIZE))   /**< @brief  NVM key store size (in bytes) */

    #define HSE_AES_KEY_BITS_LENS         {128U, 192U, 256U}   /**< @brief  AES key bit length (set to zero to disable a AES key size)*/

    #define HSE_MAX_SHARED_SECRET_BITS_LEN (4096U)             /**< @brief  Max shared secret bit length */

#ifdef HSE_SPT_HMAC
    #define HSE_MIN_HMAC_KEY_BITS_LEN     (128U)               /**< @brief  Min HMAC key bit length  */
    #define HSE_MAX_HMAC_KEY_BITS_LEN     (1024U)              /**< @brief  Max HMAC key bit length  */
#endif

#ifdef HSE_SPT_ECC
    #define HSE_MIN_ECC_KEY_BITS_LEN      (192U)               /**< @brief  Min ECC key bit length  */
    #define HSE_MAX_ECC_KEY_BITS_LEN      (521U)               /**< @brief  Max ECC key bit length */
#endif

#ifdef HSE_SPT_RSA
    #define HSE_MIN_RSA_KEY_BITS_LEN      (1024U)              /**< @brief  Min RSA key bit length */
    #define HSE_MAX_RSA_KEY_BITS_LEN      (4096U)              /**< @brief  Max RSA key bit length  */
    #define HSE_MAX_RSA_PUB_EXP_SIZE      (8U)                 /**< @brief  Max RSA public exponent size (in bytes) */
#endif

#ifdef HSE_SPT_CLASSIC_DH
    #define HSE_MIN_CLASSIC_DH_BITS_LEN    (1024U)              /**< @brief  Min Classic DH key bit length  */
    #define HSE_MAX_CLASSIC_DH_BITS_LEN    (4096U)              /**< @brief  Max Classic DH key bit length*/
#endif

#ifdef HSE_SPT_FAST_CMAC
    #define HSE_DEFAULT_MIN_FAST_CMAC_TAG_BITLEN   (64U)         /**< @brief  FAST CMAC default min bit length*/
#endif

#ifdef HSE_SPT_SIPHASH
    #define HSE_SIPHASH_KEY_BIT_LENS          (128U)             /**< @brief  SipHash key bit length*/
#endif

#if defined(HSE_SPT_ECDSA) || defined(HSE_SPT_EDDSA) || defined(HSE_SPT_RSASSA_PSS) || defined(HSE_SPT_RSASSA_PKCS1_V15)
    #define HSE_SPT_SIGN
#endif

#if defined(HSE_SPT_AEAD_GCM) || defined(HSE_SPT_AEAD_CCM)
    #define HSE_SPT_AEAD
#endif

#if defined(HSE_SPT_ECDH) || defined(HSE_SPT_CLASSIC_DH) || defined(HSE_SPT_MONTDH)
    #define HSE_SPT_COMPUTE_DH
#endif

#if (defined(HSE_SPT_SHA2_224) || defined(HSE_SPT_SHA2_256) || defined(HSE_SPT_SHA2_384) || \
     defined(HSE_SPT_SHA2_512) || defined(HSE_SPT_SHA2_512_224) || defined(HSE_SPT_SHA2_512_256))
    #define HSE_SPT_SHA2
#endif

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* HSE_B_CONFIG_H */

/**< @} */
