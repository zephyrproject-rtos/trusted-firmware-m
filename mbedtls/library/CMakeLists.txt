option(USE_STATIC_MBEDTLS_LIBRARY "Build mbed TLS static library." ON)
option(USE_SHARED_MBEDTLS_LIBRARY "Build mbed TLS shared library." OFF)
option(LINK_WITH_PTHREAD "Explicitly link mbed TLS library to pthread." OFF)
option(LINK_WITH_TRUSTED_STORAGE "Explicitly link mbed TLS library to trusted_storage." OFF)

# Set the project root directory if it's not already defined, as may happen if
# the library folder is included directly by a parent project, without
# including the top level CMakeLists.txt.
if(NOT DEFINED MBEDTLS_DIR)
    set(MBEDTLS_DIR ${CMAKE_SOURCE_DIR})
endif()

set(src_crypto
    aes.c
    aesni.c
    arc4.c
    aria.c
    asn1parse.c
    asn1write.c
    base64.c
    bignum.c
    blowfish.c
    camellia.c
    ccm.c
    chacha20.c
    chachapoly.c
    cipher.c
    cipher_wrap.c
    cmac.c
    ctr_drbg.c
    des.c
    dhm.c
    ecdh.c
    ecdsa.c
    ecjpake.c
    ecp.c
    ecp_curves.c
    entropy.c
    entropy_poll.c
    error.c
    gcm.c
    havege.c
    hkdf.c
    hmac_drbg.c
    md.c
    md2.c
    md4.c
    md5.c
    memory_buffer_alloc.c
    nist_kw.c
    oid.c
    padlock.c
    pem.c
    pk.c
    pk_wrap.c
    pkcs12.c
    pkcs5.c
    pkparse.c
    pkwrite.c
    platform.c
    platform_util.c
    poly1305.c
    psa_crypto.c
    psa_crypto_se.c
    psa_crypto_slot_management.c
    psa_crypto_storage.c
    psa_its_file.c
    ripemd160.c
    rsa.c
    rsa_internal.c
    sha1.c
    sha256.c
    sha512.c
    threading.c
    timing.c
    version.c
    version_features.c
    xtea.c
)

list(APPEND src_crypto ${thirdparty_src})

set(src_x509
    certs.c
    pkcs11.c
    x509.c
    x509_create.c
    x509_crl.c
    x509_crt.c
    x509_csr.c
    x509write_crt.c
    x509write_csr.c
)

set(src_tls
    debug.c
    net_sockets.c
    ssl_cache.c
    ssl_ciphersuites.c
    ssl_cli.c
    ssl_cookie.c
    ssl_msg.c
    ssl_srv.c
    ssl_ticket.c
    ssl_tls.c
)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wno-documentation-deprecated-sync -Wunreachable-code")
endif(CMAKE_COMPILER_IS_CLANG)

if(WIN32)
    set(libs ${libs} ws2_32)
endif(WIN32)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

if(HAIKU)
    set(libs ${libs} network)
endif(HAIKU)

if(USE_PKCS11_HELPER_LIBRARY)
    set(libs ${libs} pkcs11-helper)
endif(USE_PKCS11_HELPER_LIBRARY)

if(ENABLE_ZLIB_SUPPORT)
    set(libs ${libs} ${ZLIB_LIBRARIES})
endif(ENABLE_ZLIB_SUPPORT)

if(LINK_WITH_PTHREAD)
    set(libs ${libs} pthread)
endif()

if(LINK_WITH_TRUSTED_STORAGE)
    set(libs ${libs} trusted_storage)
endif()

if (NOT USE_STATIC_MBEDTLS_LIBRARY AND NOT USE_SHARED_MBEDTLS_LIBRARY)
    message(FATAL_ERROR "Need to choose static or shared mbedtls build!")
endif(NOT USE_STATIC_MBEDTLS_LIBRARY AND NOT USE_SHARED_MBEDTLS_LIBRARY)

if(NOT DEFINED mbedtls_target)
    set(mbedtls_target "mbedtls")
    set(mbedtls_target "mbedtls" PARENT_SCOPE)
endif()

if(NOT DEFINED mbedx509_target)
    set(mbedx509_target "mbedx509")
    set(mbedx509_target "mbedx509" PARENT_SCOPE)
endif()

if(NOT DEFINED mbedcrypto_target)
    set(mbedcrypto_target "mbedcrypto")
    set(mbedcrypto_target "mbedcrypto" PARENT_SCOPE)
endif()

if (USE_STATIC_MBEDTLS_LIBRARY)
    if(NOT DEFINED mbedtls_static_target)
        set(mbedtls_static_target ${mbedtls_target})
        set(mbedtls_static_target ${mbedtls_target} PARENT_SCOPE)
    endif()

    if(NOT DEFINED mbedx509_static_target)
        set(mbedx509_static_target ${mbedx509_target})
        set(mbedx509_static_target ${mbedx509_target} PARENT_SCOPE)
    endif()

    if(NOT DEFINED mbedcrypto_static_target)
        set(mbedcrypto_static_target ${mbedcrypto_target})
        set(mbedcrypto_static_target ${mbedcrypto_target} PARENT_SCOPE)
    endif()
endif()

if(USE_STATIC_MBEDTLS_LIBRARY AND USE_SHARED_MBEDTLS_LIBRARY)
    string(APPEND mbedtls_static_target "_static")
    string(APPEND mbedx509_static_target "_static")
    string(APPEND mbedcrypto_static_target "_static")
endif()


if(USE_STATIC_MBEDTLS_LIBRARY)
    add_library(${mbedcrypto_static_target} STATIC ${src_crypto})
    set_target_properties(${mbedcrypto_static_target} PROPERTIES OUTPUT_NAME mbedcrypto)
    target_link_libraries(${mbedcrypto_static_target} ${libs})
    target_include_directories(${mbedcrypto_static_target}
        PUBLIC ${MBEDTLS_DIR}/include/
        PUBLIC ${MBEDTLS_DIR}/include/mbedtls/
    )

    add_library(${mbedx509_static_target} STATIC ${src_x509})
    set_target_properties(${mbedx509_static_target} PROPERTIES OUTPUT_NAME mbedx509)
    target_link_libraries(${mbedx509_static_target} ${libs} ${mbedcrypto_static_target})

    add_library(${mbedtls_static_target} STATIC ${src_tls})
    set_target_properties(${mbedtls_static_target} PROPERTIES OUTPUT_NAME mbedtls)
    target_link_libraries(${mbedtls_static_target} PUBLIC ${libs} ${mbedx509_static_target})

    if(INSTALL_MBEDTLS)
        install(TARGETS ${mbedtls_static_target} ${mbedx509_static_target} ${mbedcrypto_static_target}
                DESTINATION ${LIB_INSTALL_DIR}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
endif(USE_STATIC_MBEDTLS_LIBRARY)

if(USE_SHARED_MBEDTLS_LIBRARY)

    add_library(${mbedcrypto_target} SHARED ${src_crypto})
    set_target_properties(${mbedcrypto_target} PROPERTIES VERSION 2.23.0 SOVERSION 5)
    target_link_libraries(${mbedcrypto_target} ${libs})
    target_include_directories(${mbedcrypto_target}
        PUBLIC ${MBEDTLS_DIR}/include/
        PUBLIC ${MBEDTLS_DIR}/include/mbedtls/
    )

    add_library(${mbedx509_target} SHARED ${src_x509})
    set_target_properties(${mbedx509_target} PROPERTIES VERSION 2.23.0 SOVERSION 1)
    target_link_libraries(${mbedx509_target} ${libs} ${mbedcrypto_target})
    target_include_directories(${mbedx509_target}
        PUBLIC ${MBEDTLS_DIR}/include/)

    add_library(${mbedtls_target} SHARED ${src_tls})
    set_target_properties(${mbedtls_target} PROPERTIES VERSION 2.23.0 SOVERSION 13)
    target_link_libraries(${mbedtls_target} PUBLIC ${libs} ${mbedx509_target})
    target_include_directories(${mbedtls_target}
        PUBLIC ${${mbedtls_target}_DIR}/include/)

    if(INSTALL_MBEDTLS)
        install(TARGETS ${mbedtls_target} ${mbedx509_target} ${mbedcrypto_target}
                DESTINATION ${LIB_INSTALL_DIR}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
endif(USE_SHARED_MBEDTLS_LIBRARY)

if(NOT DEFINED lib_target)
    set(lib_target lib)
endif()

add_custom_target(${lib_target} DEPENDS ${mbedcrypto_target} ${mbedx509_target} ${mbedtls_target})
if(USE_STATIC_MBEDTLS_LIBRARY AND USE_SHARED_MBEDTLS_LIBRARY)
    add_dependencies(${lib_target} ${mbedcrypto_static_target} ${mbedx509_static_target} ${mbedtls_static_target})
endif()
