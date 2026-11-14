#ifndef ZN_CRYPTO_H
#define ZN_CRYPTO_H

#define ZN_CRYPTO_SHA256_DIGEST_SIZE 32

#ifdef ZN_WITH_SSL

#ifdef __cplusplus
extern "C" {
#endif


ZN_DLL_EXPORT bool ZN_Crypto_RandomBytes(
    uint8_t *buffer,
    size_t size
);

ZN_DLL_EXPORT bool ZN_Crypto_SHA256(
    const uint8_t *_data,
    size_t _size,
    uint8_t _digest[ZN_CRYPTO_SHA256_DIGEST_SIZE]
);

#ifdef __cplusplus
}
#endif


#endif

#endif
