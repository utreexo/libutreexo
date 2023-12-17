// The MIT License (MIT)

// Copyright (c) 2023 Davidson Souza

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

/**
 * A utreexo implementation in pure C99. This implementation is meant to be a
 * small, portable and easy to use implementation of utreexo. It also implements
 * an optimized version of the forest that uses memory mapped files to store the
 * forest. This is useful if you don't have enough memory to store the whole
 * forest in memory, or don't want to pay the memory cost of storing the forest
 * specially since many nodes will be accessed very rarely.
 *
 * This library also (TODO) implements the verification of proofs, and updating
 * the accumulator, given a proof and the leaf that was added/removed.
 *
 *
 * To navigate the API, here are some rules:
 *
 * 1 - Output pointers always come first. If the function has one or more
 *     outputs, they will be made using a user provided pointer. No
 *     allocation is required for this pointer, the only invariant is that they
 *     are valid pointers.
 *
 * 2 - Arrays always have a length argument immediately after it in the
 *     argument list.
 *
 * 3 - Some functions that takes arrays as arguments may allow NULL pointers
 *     for the array, in this case, the length argument must be 0.
 *
 * 4 - All functions that may fail are marked with MUST_USE, so the compiler
 *     will warn you if you don't check the return value.
 */

#ifndef UTREEXO_H
#define UTREEXO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * A utreexo node hash. This is just a 32-byte array.
 */
typedef struct {
  uint8_t data[32];
} utreexo_node_hash;

/**
 * A utreexo forest. This is the main type of this module. It represents a
 * forest of trees. The forest is stored in a file, that is then memory mapped,
 * we can leverage the OS's virtual memory system to optimize the memory usage
 * keeping only hot nodes in memory.
 *
 * This type is opaque, so you can't access it's fields directly, instead, you
 * should use the functions provided by this module. You can modify, prove
 * elements, serialize and deserialize the forest using the functions provided
 */
typedef struct utreexo_forest_ *utreexo_forest;

/**
 * Creates a new forest. A forest is a collection of trees that are stored in
 * a file that is memory mapped. This allows the forest to be larger than the
 * available memory. The forest is stored in a file, and the roots of the trees
 * are stored in memory. The roots are stored in an array of 64 roots, so the
 * forest can have up to 64 trees (which is more than enough for most use
 * cases).
 *
 * This method returns 0 if everything goes Ok, 1 otherwise.
 *
 * Out:         p: The newly created forest
 * In:   filename: File name of the forest backend. If the file doesn't exist,
 *                 it'll be created.
 */
extern int utreexo_forest_init(utreexo_forest *p, const char *map_name,
                               const char *forest_name);

/**
 * Frees-up a forest. This method should be called when you're done with
 * the forest, otherwise may cause resource leak.
 *
 * This method doesn't fail.
 *
 * In:  p: A valid forest that have been innitialized using
 * utreexo_forest_init
 */
extern int utreexo_forest_free(utreexo_forest p);

/**
 * Modify is the main interface for a utreexo forest. Because order matters
 * calling add/delete directly is not allowed. Instead, modify should be used
 * so we can guarantee that the forest is always in a valid state. This function
 * takes as arguments the leaf that should be added/removed, and the number of
 * leaves for each operation.
 *
 * This method returns 0 if everything goes Ok, 1 otherwise.
 *
 * Out:           p: The newly created forest
 * In:         leaf: The leaf that should be added
 *       leaf_count: The number of leaves that should be added/removed
 */
extern int utreexo_forest_modify(utreexo_forest forest,
                                 utreexo_node_hash *utxos, int utxo_count,
                                 utreexo_node_hash *stxos, int stxo_count);

/**
 * Prove that some elements are in the forest. This function takes as input
 * an array of leaves, and an array that will be filled with the proofs.
 *
 * Out:           p: The newly created forest
 * In:         leaf: The leaf that should be added
 *       leaf_count: The number of leaves that should be added/removed
 */
static int utreexo_forest_prove(utreexo_forest forest, utreexo_node_hash *leaf,
                                int leaf_count, utreexo_node_hash *proof);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif
