#include <stddef.h>
#include <stdint.h>

/**
 * Poorly-ordered struct (lots of padding waste).
 * Members: char, int, char, double, char
 */
struct padded {
    char   a;    /* 1 byte + 3 padding */
    int    b;    /* 4 bytes */
    char   c;    /* 1 byte + 7 padding */
    double d;    /* 8 bytes */
    char   e;    /* 1 byte + 7 padding to align struct */
};

/**
 * Optimized struct — reorder largest to smallest to minimize padding.
 */
struct optimized {
    double d;    /* 8 bytes */
    int    b;    /* 4 bytes */
    char   a;    /* 1 byte */
    char   c;    /* 1 byte */
    char   e;    /* 1 byte + 1 padding to align struct to 8 */
};

/**
 * Packed struct — no padding whatsoever.
 */
struct packed {
    char   a;
    int    b;
    char   c;
    double d;
    char   e;
} __attribute__((packed));

/**
 * @brief Return sizeof the poorly-ordered struct.
 */
size_t get_padded_size(void) {
    /* TODO: Implement your solution here */
    return 0;
}

/**
 * @brief Return sizeof the optimized (reordered) struct.
 */
size_t get_optimized_size(void) {
    /* TODO: Implement your solution here */
    return 0;
}

/**
 * @brief Return sizeof the packed struct.
 */
size_t get_packed_size(void) {
    /* TODO: Implement your solution here */
    return 0;
}

/**
 * @brief Compute padding bytes needed to align a member.
 *
 * @param offset     Current byte offset within the struct.
 * @param alignment  Required alignment (power of 2).
 * @return           Number of padding bytes needed (0 if already aligned).
 */
int compute_padding(int offset, int alignment) {
    /* TODO: Implement your solution here */
    (void)offset;
    (void)alignment;
    return 0;
}

/**
 * @brief Check if a pointer is 64-byte (cache-line) aligned.
 *
 * @param ptr  Pointer to check.
 * @return     1 if aligned to 64 bytes, 0 otherwise.
 */
int is_cache_aligned(void *ptr) {
    /* TODO: Implement your solution here */
    (void)ptr;
    return 0;
}

/**
 * @brief Compute the next aligned offset.
 *
 * @param current_offset  Current byte offset.
 * @param alignment       Required alignment (power of 2).
 * @return                Smallest offset >= current_offset that is a multiple of alignment.
 */
int get_aligned_offset(int current_offset, int alignment) {
    /* TODO: Implement your solution here */
    (void)current_offset;
    (void)alignment;
    return 0;
}
