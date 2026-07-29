# 022. Const Semantics & ROM Placement

## Difficulty: Medium

## Problem Statement

In embedded systems, understanding `const` semantics is critical for controlling memory placement (RAM vs. ROM/.rodata) and expressing programmer intent for safety. This question tests your understanding of `const` qualifiers applied to pointers and data.

You must implement the following functions:

1. `modify_through_ptr(const int *ptr)` — Attempts to return the value pointed to by `ptr` multiplied by 2. You receive a pointer-to-const; you must NOT modify the pointed-to value. Return the computed value.

2. `advance_pointer(int *const *ptr_to_const_ptr, int offset)` — You receive a pointer to a const pointer (the pointer itself cannot be reassigned). Read the value at `(*ptr_to_const_ptr)[offset]` and return it.

3. `sum_rom_table(const int *table, int len)` — Sum all elements in a const array (simulating reading from ROM/.rodata). Return the sum.

4. `swap_const_ptrs(const int **pp1, const int **pp2)` — Swap what two pointer-to-const-int variables point to. The pointed-to values are const (you can't modify them), but you can reassign the pointers themselves.

5. `get_readonly_lut_value(int index)` — Return a value from a static const lookup table (demonstrating .rodata placement). The LUT contains: `{10, 20, 30, 40, 50, 60, 70, 80}`. Return -1 for out-of-range index.

## Function Signatures

```c
int modify_through_ptr(const int *ptr);
int advance_pointer(int *const *ptr_to_const_ptr, int offset);
int sum_rom_table(const int *table, int len);
void swap_const_ptrs(const int **pp1, const int **pp2);
int get_readonly_lut_value(int index);
```

## Parameters

### modify_through_ptr
| Parameter | Type         | Description                                    |
|-----------|--------------|------------------------------------------------|
| `ptr`     | `const int *`| Pointer to a const int (cannot modify target)  |

### advance_pointer
| Parameter           | Type              | Description                              |
|---------------------|-------------------|------------------------------------------|
| `ptr_to_const_ptr`  | `int *const *`    | Pointer to a const pointer to int        |
| `offset`            | `int`             | Offset from the base pointer to read     |

### sum_rom_table
| Parameter | Type         | Description                              |
|-----------|--------------|------------------------------------------|
| `table`   | `const int *`| Pointer to a read-only array of ints     |
| `len`     | `int`        | Number of elements in the array          |

### swap_const_ptrs
| Parameter | Type           | Description                            |
|-----------|----------------|----------------------------------------|
| `pp1`     | `const int **` | Pointer to first pointer-to-const-int  |
| `pp2`     | `const int **` | Pointer to second pointer-to-const-int |

### get_readonly_lut_value
| Parameter | Type  | Description                     |
|-----------|-------|---------------------------------|
| `index`   | `int` | Index into the static const LUT |

## Return Values

- `modify_through_ptr`: `*ptr * 2` (doubled value without modifying the original)
- `advance_pointer`: value at `(*ptr_to_const_ptr)[offset]`
- `sum_rom_table`: sum of all elements in the table
- `swap_const_ptrs`: void (swaps via pointer reassignment)
- `get_readonly_lut_value`: LUT value at index, or -1 if out of range (0-7 valid)

## Examples

### Example 1
```
int x = 5;
modify_through_ptr(&x);  → returns 10 (x remains 5)
```

### Example 2
```
int arr[] = {10, 20, 30};
int *p = arr;
advance_pointer(&p, 2);  → returns 30
```

### Example 3
```
const int rom_data[] = {1, 2, 3, 4};
sum_rom_table(rom_data, 4);  → returns 10
```

### Example 4
```
const int a = 100, b = 200;
const int *pa = &a, *pb = &b;
swap_const_ptrs(&pa, &pb);
// Now *pa == 200, *pb == 100
```

## Constraints

- `modify_through_ptr`: ptr is never NULL
- `advance_pointer`: offset is always valid for the underlying array
- `sum_rom_table`: len >= 0, table may be NULL if len == 0
- `get_readonly_lut_value`: valid indices are 0-7

## Notes

- `const int *p` (pointer to const): you can change `p` but not `*p`.
- `int *const p` (const pointer): you cannot change `p` but can change `*p`.
- `const int *const p` (const pointer to const): you cannot change either.
- Compilers place `static const` data in `.rodata` (flash/ROM on MCUs).
- The `const` qualifier is a compile-time contract — it cannot be cast away safely in embedded systems where data may reside in actual read-only memory.
