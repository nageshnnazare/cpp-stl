# `std::bitset<N>` — a compile-time pack of N bits

A `bitset` is not a container in the STL sense (no iterators
until C++26 proposals; you index bits). It is the right type
when N is **known at compile time** and you want word-wise
`& | ^ <<` and `count()`.

```
  bitset<8> b{0b10110010};

  index:  7 6 5 4 3 2 1 0     (0 is the least significant bit)
  bits:   1 0 1 1 0 0 1 0
```

Header: `<bitset>`. N is part of the type: `bitset<8>` and
`bitset<9>` cannot be assigned. Heap: none. Size: about
`ceil(N/word)` words inside the object.

## CRUD (bit-shaped)

| | Methods | Cost |
| --- | --- | --- |
| C | `bitset<N>{}`, `{0b...}`, `bitset<N>(val)`, `bitset<N>(string)` | O(N / word) |
| R | `test(i)`, `operator[]`, `count`, `any`, `none`, `all`, `to_ulong` / `to_ullong` / `to_string` | O(1) bit; `count` O(N/word) |
| U | `set`, `reset`, `flip`, `&=`, `\|=`, `^=`, `<<=`, `>>=` | O(N/word) for whole-set ops |
| D | `reset(i)` or `reset()` | you cannot shrink N |

`[]` on a non-const bitset is a proxy (like `vector<bool>`).
`test(i)` throws if `i >= N`; `[]` does not have to.

## Job 1 — flags without a `vector<char>`

```cpp
enum : std::size_t { Read = 0, Write = 1, Exec = 2 };
std::bitset<3> perm;
perm.set(Read).set(Exec);
if (perm.test(Write)) { }
```

**Why not `unsigned` masks?** Fine for ≤ 64 flags and when you
like `1u << k`. `bitset<200>` is the same idea past 64 bits
without rolling your own words. **Why not `vector<bool>`?**
N is fixed; `bitset` is on the stack and has `& | ^`.

## Job 2 — sieve / visited in a small universe

```
  N = 100
  bitset<101> composite;
  for (p = 2; p*p <= N; ++p)
      if (!composite[p])
          for (m = p*p; m <= N; m += p) composite.set(m);
```

A `vector<char>` is clearer when N is runtime. `bitset` wins
when N is a constant and you pass the object by value (it is
tiny) or you need `count()` of set bits often.

## Job 3 — subset as an integer key (small N)

`to_ulong()` turns a `bitset<20>` into a `size_t` for a DP
index or an `unordered_map` key. N must fit the target integer
or `to_ulong` throws `overflow_error`.

## Why this, not that

| Alternative | Prefer bitset | Prefer the other |
| --- | --- | --- |
| `unsigned long` | N > 64 or you want `.count()` / `.test()` | N ≤ 64 and C-style APIs |
| `vector<char>` | N compile-time, bitwise ops | N runtime, you need iterators |
| `vector<bool>` | always, if N is fixed | never as a real container |
| `set<int>` of set bits | sparse? then **set** of indexes | dense flags: bitset |

No `push_back`. No `find` of a pattern (roll your own, or use
a `string` of 0/1 and `search`). Not a replacement for
`unordered_set` of arbitrary ints.

## Related problems

- Subsets of n ≤ 20 items (`1 << n` or `bitset`)
- N-queens bit masks, sudoku bit rows
- Hamming distance (`(a ^ b).count()`)
- Permission bits, feature flags
- Sieve of Eratosthenes (small N)

Run [`examples/19_bitset_flags.cpp`](../examples/19_bitset_flags.cpp).
