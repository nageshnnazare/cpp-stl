# `std::array` — compile-time length, zero heap

`std::array<T, N>` is a C array that knows its size and has
iterators. `N` is part of the **type**.

```
  array<int, 4> a = {8, 1, 3, 2};

  stack (typical):   [ 8 | 1 | 3 | 2 ]
                       0   1   2   3
  sizeof(a) == 4 * sizeof(int)     no new[], no delete[]
```

Header: `<array>`. Iterator: **contiguous**.
Passes into C APIs via `a.data()`.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `{}`, `{...}`, copy | O(N) |
| R | `[]`, `at`, `front`, `back`, `data`, `size` (constexpr) | O(1) |
| U | `a[i]=`, `fill`, `swap` | O(1) / O(N) |
| D | none; overwrite or let the object die | — |

No `push_back`. If you need to grow, you wanted `vector`.

## Job 1 — fixed lookup tables

```cpp
constexpr std::array<const char*, 7> dow{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
return dow.at(idx);
```

**Why not `vector<string>`?** Heap, no constexpr size, slower
startup. **Why not raw `const char* dow[7]`?** Raw arrays decay
to pointers and lose length at the first function call.

## Job 2 — small histograms / counters

256 buckets for a byte, 26 for letters, 4 for DNA:

```
  count['c' - 'a']++      array<int, 26>
  [ a:2 | b:0 | c:4 | ... ]
```

An `unordered_map<char,int>` here is a hash + node for a domain
you already know is 26 wide.

## Job 3 — returning multiple values without a struct name

`array<int, 2>` as a pair of ints you can structured-bind. Prefer
a real `struct` when the fields have names.

## Why this, not that

| Alternative | Use array instead when | Use the other when |
| --- | --- | --- |
| C array `T[N]` | you want `.size()`, copy, range-for | interfacing with C that *requires* `T[]` |
| `vector` | N is a constant and small | N is runtime |
| `span` | you **own** the N elements | you only **view** them |

`array<T, 0>` is a valid empty type. `array<bool, N>` does **not**
have `vector<bool>`'s proxy weirdness.

## Related problems

- Count letters / anagram signature (`array<int,26>` as a key —
  see [`recipes/two_sum_and_anagrams.md`](../recipes/two_sum_and_anagrams.md))
- Rotate a fixed image / matrix in place
- Knight dialer, chess board `array<array<int,8>,8>`
- Precomputed sin table, CRC table

Run [`examples/05_array_histogram.cpp`](../examples/05_array_histogram.cpp).
