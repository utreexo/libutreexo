#include <stdint.h>

#include "parent_hash.h"
#include "util.h"

typedef struct {
    utreexo_sha512_256 roots[64];
    uint8_t size;
} roots;

typedef struct {
    uint64_t num_leaves;
    roots merkle_roots;
} stump;

stump add(stump s, utreexo_sha512_256 add);
stump del(stump s, proof hash_proof);
int verify(stump s, utreexo_sha512_256* hashes, proof hash_proof);
