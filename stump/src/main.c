//// src/main.c
//#include <stdio.h>
//#include <stdint.h>
//
//#include "parent_hash.h"
//#include "util.h"
//
//int main() {
//    uint8_t left[32] = {1};
//    printf("left %s\n", left);
//    hash_from_u8(left, 1);
//    printf("left %s\n", left);
//
//    //uint8_t right[32];
//    //hash_from_u8(right, 1);
//
//    //uint8_t parent[32];
//    //parent_hash(parent, left, right);
//    //printf("Hello, CMake! %s\n", parent);
//
//    //printf("treerows %d\n", tree_rows(8));
//    calculate_roots(left, 10, left, );
//    return 0;
//}

#include <stdio.h>
#include <stdlib.h>

#include "stump.h"
#include "parent_hash.h"

void makeforest(utreexo_sha512_256* forest, uint64_t numleaves) {
    int total = (numleaves*2)-1;
    uint8_t total_rows = tree_rows(numleaves);
    for (uint64_t i = 0; i < total; i++) {
        if (i < numleaves) {
            hash_from_u8(forest[i].hash, i);
            continue;
        }

        utreexo_sha512_256 left = forest[left_child(i, total_rows)];
        utreexo_sha512_256 right = forest[right_child(i, total_rows)];
        parent_hash(forest[i].hash, left.hash, right.hash);
    }
}

void prove(utreexo_sha512_256* forest, proof* my_proof, uint64_t num_leaves) {
    uint8_t total_rows = tree_rows(num_leaves);
    uint64_t* proof_pos = (uint64_t*)malloc((my_proof->target_count*(total_rows+1)) * sizeof(uint64_t));
    int proof_count = proof_positions(my_proof->target_count, my_proof->targets, num_leaves, total_rows, proof_pos);

    printf("proof count %d\n", proof_count);
    for (uint64_t i = 0; i < proof_count; i++) {
        printf("proof pos %ld\n", proof_pos[i]);
    }

    my_proof->proof_count = proof_count;
    my_proof->proof_hashes = (utreexo_sha512_256*)malloc(proof_count * sizeof(utreexo_sha512_256));
    for (size_t i = 0; i < proof_count; i++) {
        my_proof->proof_hashes[i] = forest[proof_pos[i]];
    }
    free(proof_pos);
}

void printforest(utreexo_sha512_256* forest, uint64_t numleaves) {
    int total = (numleaves*2)-1;

    printf("total: %d\n", total);
    for (uint64_t i = 0; i < total; i++) {
        printf("%ld. %s\n", i,
                uint8_array_to_hex_string(forest[i].hash));
    }
}

int main() {
    uint64_t numleaves = 16;
    uint8_t total_rows = tree_rows(numleaves);
    int total = (numleaves*2)-1;
    utreexo_sha512_256* forest = (utreexo_sha512_256*)malloc(total * sizeof(utreexo_sha512_256));

    makeforest(forest, numleaves);
    printforest(forest, numleaves);

    proof my_proof;

    uint64_t t[] = {0};
    my_proof.targets = t;
    my_proof.target_count = sizeof(t) / sizeof(uint64_t);

    utreexo_sha512_256* delhashes = (utreexo_sha512_256*)malloc(my_proof.target_count * sizeof(utreexo_sha512_256));
    for (size_t i = 0; i < my_proof.target_count; i++) {
        delhashes[i] = forest[my_proof.targets[i]];
    }

    prove(forest, &my_proof, numleaves);

    for (uint64_t i = 0; i < my_proof.target_count; i++) {
        printf("target %ld\n", my_proof.targets[i]);
    }

    for (uint64_t i = 0; i < my_proof.proof_count; i++) {
        printf("proof %s\n",
                uint8_array_to_hex_string(my_proof.proof_hashes[i].hash));
    }

    utreexo_sha512_256 roots[64];
    int count = calculate_roots(roots, numleaves, my_proof.target_count, delhashes, my_proof);

    for (uint64_t i = 0; i < count; i++) {
        printf("calculated %s\n",
                uint8_array_to_hex_string(roots[i].hash));
    }

    stump s;
    s.merkle_roots.size = 0;
    s.num_leaves = 0;

    for (uint64_t i = 0; i < numleaves; i++) {
        utreexo_sha512_256 hash;
        hash_from_u8(hash.hash, i);
        s = add(s, hash);
    }
    for (int i = 0; i < s.merkle_roots.size; i++) {
        printf("stump %s\n", uint8_array_to_hex_string(s.merkle_roots.roots[i].hash));
    }

    s = del(s, my_proof);
    for (int i = 0; i < s.merkle_roots.size; i++) {
        printf("stump %s\n", uint8_array_to_hex_string(s.merkle_roots.roots[i].hash));
    }

    size_t* arr = (size_t*)malloc(10 * sizeof(size_t));
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 1;
    arr[3] = 1;
    arr[4] = 3;
    arr[5] = 1;
    arr[6] = 3;
    arr[7] = 1;
    arr[8] = 2;
    arr[9] = 1;

    int c = deduplicate(arr, 10);
    printf("count is %d\n", c);

    for (int i = 0; i < c; i++) {
        printf("at %d: val is %ld\n", i, arr[i]);
    }
}

//int main() {
//    // Example: Initialize proof structure with 3 targets and 3 proof hashes
//
//    size_t num_elements = 3; // Define how many targets and hashes you need
//
//    // Create and initialize a proof variable
//    proof my_proof;
//
//    my_proof.target_count = num_elements;
//    my_proof.proof_count = num_elements;
//
//    // Allocate memory for the targets array
//    my_proof.targets = (uint64_t*)malloc(num_elements * sizeof(uint64_t));
//    if (my_proof.targets == NULL) {
//        perror("Failed to allocate memory for targets");
//        return 1;
//    }
//
//    // Allocate memory for the proof_hashes array
//    my_proof.proof_hashes = (utreexo_sha512_256*)malloc(num_elements * sizeof(utreexo_sha512_256));
//    if (my_proof.proof_hashes == NULL) {
//        perror("Failed to allocate memory for proof_hashes");
//        free(my_proof.targets);  // Clean up allocated memory in case of error
//        return 1;
//    }
//
//    // Initialize the targets array
//    for (size_t i = 0; i < num_elements; i++) {
//        my_proof.targets[i] = i + 1;  // Example initialization with consecutive numbers
//        printf("%ld\n", my_proof.targets[i]);
//    }
//    my_proof.targets[0] = 7;
//
//    for (size_t i = 0; i < num_elements; i++) {
//        printf("%ld\n", my_proof.targets[i]);
//    }
//
//    printf("00 %ld\n", sizeof(uint64_t));
//    printf("01 %ld\n", sizeof(my_proof.targets));
//    printf("02 %ld\n", num_elements);
//    printf("03 %ld\n", sizeof(&my_proof.targets) / sizeof(my_proof.targets[0]));
//
//    // Initialize the proof_hashes array (example with dummy data)
//    for (size_t i = 0; i < num_elements; i++) {
//        hash_from_u8(my_proof.proof_hashes[i].hash, i);
//    }
//
//    for (size_t i = 0; i < num_elements; i++) {
//        hash_from_u8(my_proof.proof_hashes[i].hash, i);
//    }
//
//    utreexo_sha512_256* delhashes = (utreexo_sha512_256*)malloc(num_elements * sizeof(utreexo_sha512_256));
//
//    utreexo_sha512_256* calculated_roots = (utreexo_sha512_256*)malloc(256 * sizeof(utreexo_sha512_256));
//
//    calculate_roots(calculated_roots, 8, num_elements, delhashes, my_proof);
//
//    // Free allocated memory
//    free(my_proof.targets);
//    free(my_proof.proof_hashes);
//
//    return 0;
//}
