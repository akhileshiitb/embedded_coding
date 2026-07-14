#include <stdint.h>

/**
 * @brief Insert a value into a bit field defined by start and end positions.
 *
 * Clears bits [start..end] in reg and inserts value at that position.
 *
 * @param reg    The original 32-bit register value.
 * @param start  The lowest bit position of the field (inclusive).
 * @param end    The highest bit position of the field (inclusive).
 * @param value  The value to insert (unshifted, starting from bit 0).
 * @return       The register with the field updated to the new value.
 */
uint32_t insert_bits_pos(uint32_t reg, int start, int end, uint32_t value) {
    /* TODO: Implement your solution here */
    return 0;
}
