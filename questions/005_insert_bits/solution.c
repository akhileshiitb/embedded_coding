#include <stdint.h>

/**
 * @brief Insert a value into a bit field defined by a mask.
 *
 * Clears the bit field in reg (defined by mask) and inserts value
 * at the correct position determined by the mask's lowest set bit.
 *
 * @param reg    The original 32-bit register value.
 * @param mask   Bit mask defining the target field.
 * @param value  The value to insert (unshifted, starting from bit 0).
 * @return       The register with the field updated to the new value.
 */
uint32_t insert_bits(uint32_t reg, uint32_t mask, uint32_t value) {
    /* TODO: Implement your solution here */
    return 0;
}
