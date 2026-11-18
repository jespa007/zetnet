#include "zetnet.h"

#ifdef ZN_WITH_SSL

bool ZN_Crypto_RandomBytes(
    uint8_t *_buffer,
    size_t _size
){
    return RAND_bytes(_buffer, (int)_size) == 1;
}

bool ZN_Crypto_SHA256(
    const uint8_t *_data,
    size_t _size,
    uint8_t _digest[ZN_CRYPTO_SHA256_DIGEST_SIZE]
) {

    unsigned int digest_length = 0;

    if (!_digest || (!_data && _size > 0)) {
        return false;
    }

    /*
     * EVP_Digest expects a valid input pointer.
     * Use a dummy byte when hashing an empty input.
     */
    static const uint8_t empty = 0;

    if (!_data) {
        _data = &empty;
    }

    if (EVP_Digest(
            _data,
            _size,
            _digest,
            &digest_length,
            EVP_sha256(),
            NULL
        ) != 1) {
        return false;
    }

    return digest_length == ZN_CRYPTO_SHA256_DIGEST_SIZE;
}

#endif
