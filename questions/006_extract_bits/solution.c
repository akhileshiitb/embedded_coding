#include <stdint.h>

/**
 * @brief Extract a value from a bit field defined by a mask.
 *
 * Isolates the bits defined by mask in reg, then shifts the result
 * right so the field's LSB is at bit position 0.
 *
 * @param reg    The 32-bit register value to extract from.
 * @param mask   Bit mask defining the target field.
 * @return       The extracted field value, right-aligned to bit 0.
 */
uint32_t extract_bits(uint32_t reg, uint32_t mask) {
    /* TODO: Implement your solution here */
    return 0;
}
