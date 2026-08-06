# Hints

## Hint 1

XOR has a special property: `a ^ a = 0` and `a ^ 0 = a`. If you XOR all elements together, every duplicate pair cancels out, leaving you with `unique1 ^ unique2`. But how do you separate them? Think about what the set bits in this combined XOR value tell you — they represent bit positions where the two unique numbers **differ**.

## Hint 2

Once you have `xor_all = unique1 ^ unique2`, pick **any** set bit in `xor_all` (the rightmost is easiest: `xor_all & (-xor_all)`). This bit is 1 in one unique number and 0 in the other. Use this bit to **partition** all array elements into two groups. Since duplicate pairs always have the same bits, both copies land in the same group. Each group now contains exactly one unique element — XOR within each group to find it.

## Hint 3

```c
// Step 1: XOR all elements
int32_t xor_all = 0;
for (int i = 0; i < n; i++) xor_all ^= arr[i];

// Step 2: Isolate rightmost set bit (distinguishing bit)
int32_t diff_bit = xor_all & (-xor_all);

// Step 3: Partition and XOR each group
int32_t group1 = 0, group2 = 0;
for (int i = 0; i < n; i++) {
    if (arr[i] & diff_bit) group1 ^= arr[i];
    else                   group2 ^= arr[i];
}

// Step 4: Assign smaller to out1, larger to out2
```
