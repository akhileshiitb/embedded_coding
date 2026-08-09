# Hints

## Hint 1

Pointer arithmetic is scaled by the size of the pointed-to type. If you cast the integer `0` to a `type *`, you get a null pointer. Incrementing that pointer by 1 gives you an address equal to `sizeof(type)`. You just need to cast that resulting pointer back to an integer type (`size_t`) to get the byte count.

## Hint 2

The pattern for `MY_SIZEOF(type)`:
1. Cast 0 to `type *`: `(type *)0`
2. Advance by 1 element: `(type *)0 + 1`
3. Cast the resulting pointer to `size_t` to get the byte offset

For `MY_SIZEOF_VAR(var)`, you need to dereference the variable's address to get its type — think about `&var` and how pointer arithmetic on it would work. The difference `(&var + 1) - &var` in bytes gives you the size.

## Hint 3

```c
#define MY_SIZEOF(type)      ((size_t)((type *)0 + 1))

#define MY_SIZEOF_VAR(var)   ((size_t)((char *)(&var + 1) - (char *)&var))
```

`MY_SIZEOF(type)`: Cast NULL to a `type*` pointer, advance by 1, and the resulting address IS the size in bytes.

`MY_SIZEOF_VAR(var)`: Take the address of `var`, advance by 1 (moves by one `typeof(var)` worth of bytes), cast both to `char *` (byte-level addressing), and subtract to get the byte count. This works for arrays too — `&arr + 1` moves past the entire array, not just one element.
