#include <stdint.h>

/**
 * @brief Insert a value into a bit field of a 32-bit register.
 *
 * Clears the field defined by mask in reg, then inserts value
 * shifted left by shift positions.
 *
 * @param reg    The original 32-bit register value.
 * @param mask   Bit mask defining the target field.
 * @param shift  Number of bits to left-shift value before insert.
 * @param value  The value to insert (right-aligned, pre-shift).
 * @return       The register with the field updated.
 */
#define BITFIELD_INSERT(reg, mask, shift, value) (0)
