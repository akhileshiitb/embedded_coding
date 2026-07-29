# Hints

## Hint 1

Think about what each `const` qualifier restricts:
- `const int *p` — you can read `*p` but cannot write to `*p`. You CAN reassign `p`.
- `int *const p` — you cannot reassign `p`, but you CAN read/write `*p`.
- For `swap_const_ptrs`, you're swapping pointer values (not the data they point to).

## Hint 2

- `modify_through_ptr`: Simply dereference and multiply — the const prevents you from accidentally doing `*ptr = ...`.
- `advance_pointer`: Dereference the outer pointer once to get the base pointer, then use array indexing.
- `sum_rom_table`: Standard loop over `table[i]`, but handle `len == 0` gracefully.
- `swap_const_ptrs`: Use a temporary `const int *tmp` to hold one pointer, then swap.
- `get_readonly_lut_value`: Declare `static const int lut[] = {...}` inside the function — the compiler places this in `.rodata`.

## Hint 3

```c
int modify_through_ptr(const int *ptr) { return (*ptr) * 2; }

int advance_pointer(int *const *ptr_to_const_ptr, int offset) {
    return (*ptr_to_const_ptr)[offset];
}

int sum_rom_table(const int *table, int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) sum += table[i];
    return sum;
}

void swap_const_ptrs(const int **pp1, const int **pp2) {
    const int *tmp = *pp1;
    *pp1 = *pp2;
    *pp2 = tmp;
}

int get_readonly_lut_value(int index) {
    static const int lut[] = {10,20,30,40,50,60,70,80};
    if (index < 0 || index > 7) return -1;
    return lut[index];
}
```
