#include <stdint.h>

/**
 * @brief Extract a value from a bit field defined by start and end positions.
 *
 * Isolates bits [start..end] in reg and returns the value right-shifted
 * so the field's LSB is at bit position 0.
 *
 * @param reg    The 32-bit register value to extract from.
 * @param start  The lowest bit position of the field (inclusive).
 * @param end    The highest bit position of the field (inclusive).
 * @return       The extracted field value, right-aligned to bit 0.
 */
uint32_t extract_bits_pos(uint32_t reg, int start, int end) {
    /* TODO: Implement your solution here */
    return 0;
}
