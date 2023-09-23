#include <openssl/evp.h>
#include <string.h>

#ifndef UTREEXO_PARENT_HASH
#define UTREEXO_PARENT_HASH

static inline void sha512_256(uint8_t hash[32], const void *data, size_t len)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha512_256(), NULL);
    EVP_DigestUpdate(ctx, data, len);
    EVP_DigestFinal_ex(ctx, hash, NULL);
    EVP_MD_CTX_free(ctx);
}

static inline void parent_hash(uint8_t out[32], uint8_t left[32], uint8_t right[32])
{
    uint8_t concat[64];
    memcpy(concat, left, 32);
    memcpy(concat + 32, right, 32);
    sha512_256(out, concat, 64);
}

typedef struct
{
    uint8_t hash[32];
} utreexo_node_hash;

#endif