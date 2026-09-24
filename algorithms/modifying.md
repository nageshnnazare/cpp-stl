# Modifying algorithms — copy, map, drop, rearrange

These write through output iterators or permute the range.
Header: `<algorithm>`. Time O(n). Extra space O(1) unless the
output is a second container you grow.

```
  transform  x |-> 2x
  [ 1 | 2 | 3 ]     ->    [ 2 | 4 | 6 ]

  remove  odds (logical)
  [ 1 | 2 | 3 | 4 ] ->    [ 2 | 4 | ? | ? ]  + erase tail
```

## Copy / move / fill / generate

| Algorithm | Writes | Notes |
| --- | --- | --- |
| `copy` / `copy_n` / `copy_if` | to an output iterator | output must already have room **or** be an inserter |
| `copy_backward` | overlapping right-shift | dest_last is the *end* of dest |
| `move` / `move_backward` | steal | source left in moved-from state |
| `fill` / `fill_n` | a constant | |
| `generate` / `generate_n` | `g()` each time | RNG, iota-like |
| `transform` | `f(x)` or `f(x,y)` | the map of STL |
| `replace` / `replace_if` | in place | `replace_copy` writes elsewhere |
| `iter_swap` / `swap_ranges` | exchange | |

```cpp
std::vector<int> b;
std::copy_if(a.begin(), a.end(), std::back_inserter(b),
             [](int x){ return x > 0; });
```

Without `back_inserter`, `copy` into empty `b.begin()` is UB.

## The erase-remove idiom (the one mutating pattern you must own)

`remove` / `remove_if` **compact keepers to the front** and
return the new logical end. Size does not change until `erase`.

```
  remove_if  (x < 0)
  [ 3 | -1 | 4 | -2 | 5 ]
        ^ walk
  [ 3 |  4 | 5 | -2 | 5 ]
                 ^ returned iterator
  v.erase(that, v.end())
  [ 3 | 4 | 5 ]
```

```cpp
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

On `list` / `forward_list`, prefer the **member** `remove` /
`remove_if`: they relink nodes in O(n) without the compact+erase
dance (and for `list`, without moving `T`).

**Why not `erase` inside the loop?**

```
  for (it = v.begin(); it != v.end(); )
      if (pred(*it)) it = v.erase(it); else ++it;
```

Correct on `vector` but O(n²) moves. On `unordered_map` this
pattern *is* the right one (`erase(it)` is O(1) avg and returns
the next iterator in C++11). Match the idiom to the container.

## `unique` — adjacent duplicates only

```
  [ 1 | 1 | 2 | 2 | 2 | 3 ]  unique ->  [ 1 | 2 | 3 | ? | ? | ? ]
  [ 1 | 2 | 1 ]              unique does NOTHING to the two 1s
```

Sort first if you want global uniqueness:

```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
```

**Why not `set`?** `set` uniques *while* inserting, O(n log n)
with node alloc. Sort+unique is O(n log n) contiguous and usually
faster for a one-shot.

## Rotate, reverse, shift, shuffle, sample

```
  rotate left by 2
  [ A B C D E ]  ->  [ C D E A B ]
         ^ new first (the iterator you pass)

  reverse         [ E D C B A ]
```

`std::rotate(first, middle, last)`: `middle` becomes the new
`first`. Implement "push_front on a vector of known cap" as
rotate if you must stay contiguous.

`shuffle` needs a URNG (`mt19937`). `random_shuffle` is gone
(C++17). `sample` copies k random elements **without** shuffling
the source (C++17).

## Critical thinking

| Need | Algorithm | Not |
| --- | --- | --- |
| map values 1:1 | `transform` | `for_each` + `push_back` unless you already reserved and index |
| filter to a new vector | `copy_if` | `remove_if` on a copy if you still need the original |
| filter in place | `remove_if` + `erase` | `copy_if` + swap if allocation is OK |
| unique keys | `sort`+`unique` or `unordered_set` | `unique` alone |
| stable unique (keep first occurrence order) | `unordered_set seen` + `copy_if` | `sort`+`unique` (loses order) |

## Related problems

- Move zeroes (`remove` 0 then `fill` tail, or two pointers)
- Remove duplicates from sorted array (`unique`)
- Rotate array (189)
- Apply permutation / `transform` a codebook
- Random pick (`sample` or reservoir)

Run [`examples/21_algo_mutate.cpp`](../examples/21_algo_mutate.cpp).
