# Algorithm × iterator requirements

Every algorithm is compiled against the **weakest** iterator
that makes its inner loop legal. If your container's iterator
is weaker, you get a type error (`sort` + `list`) or a silent
O(n) disaster (`lower_bound` + `set`).

```
  input    --find, count, copy from, accumulate-->
  output   --copy to, fill, generate-->
  forward  --unique, rotate, lower_bound (hops O(n) if not RA)-->
  bidi     --reverse, next_permutation, inplace_merge-->
  rand     --sort, nth_element, heap, random-access lower_bound-->
  contig   --data(), C APIs, memcpy-shaped copy-->
```

`F` = forward, `B` = bidirectional, `R` = random-access,
`C` = contiguous (implies R). In = input, Out = output.
Complexity assumes the iterator can do what the table claims
in O(1) per step; a bidirectional `lower_bound` still does
O(log n) **compares** and O(n) **increments**.

## Query

| Algorithm | Needs | Notes |
| --- | --- | --- |
| `for_each` / `for_each_n` | In | |
| `count` / `count_if` | In | |
| `find` / `find_if` / `find_if_not` | In | |
| `find_end` / `search` | F | multi-pass on the haystack |
| `find_first_of` | F | |
| `adjacent_find` | F | |
| `mismatch` / `equal` | In | |
| `all_of` / `any_of` / `none_of` | In | |
| `search_n` | F | |
| `lexicographical_compare` | In | |
| `is_permutation` | F | extra memory or O(n²) |
| `is_sorted` / `is_sorted_until` | F | |
| `is_heap` / `is_heap_until` | R | |
| `is_partitioned` | In | |

## Mutating / copying

| Algorithm | Source | Dest | Notes |
| --- | --- | --- | --- |
| `copy` / `copy_n` / `copy_if` | In | Out | dest must have room or be an inserter |
| `copy_backward` | B | B | overlapping right |
| `move` / `move_backward` | In / B | Out / B | |
| `transform` | In | Out | |
| `fill` / `fill_n` / `generate` | F / Out | — | `generate_n` is Out |
| `replace` / `replace_if` | F | — | |
| `remove` / `remove_if` / `unique` | F | — | then container `erase` |
| `reverse` | B | — | |
| `rotate` | F | — | |
| `shuffle` | R | — | needs URNG |
| `sample` | F in, Out | R dest if n chosen | C++17 |
| `partition` | F | — | `stable_partition` F, extra RAM |
| `partition_point` | F | — | O(log n) compares; hops O(n) if not R |

## Order / select / heap / set

| Algorithm | Needs | Why |
| --- | --- | --- |
| `sort` / `stable_sort` / `partial_sort` / `nth_element` | **R** | jump to pivot / heapify |
| `make_heap` / `push_heap` / `pop_heap` / `sort_heap` | **R** | index `2i+1` |
| `lower_bound` / `upper_bound` / `equal_range` / `binary_search` | F | **use member on set/map** |
| `merge` | In + In | dest Out |
| `inplace_merge` | B | |
| `set_union` / `intersection` / `difference` / `symmetric_difference` | In | dest Out; inputs sorted |
| `includes` | In | |
| `next_permutation` / `prev_permutation` | B | |
| `min_element` / `max_element` / `minmax_element` | F | |
| `iota` | F | `<numeric>` |

## What to do when the compile fails

```
  sort(list.begin(), list.end())
        |
        v
  list has no random access
        |
        +--> lst.sort()           member merge-sort
        +--> copy to vector, sort, copy back   if you needed RA later
```

```
  lower_bound(set.begin(), set.end(), x)
        |
        v
  compiles, walks O(n)
        |
        +--> s.lower_bound(x)     always
```

`deque` **can** `sort` (it is R). It cannot give you a `T*` to
the whole buffer (not C). `vector` is the only common
sequence that is both.

Ranges versions (`std::ranges::sort(v)`) use the same
categories, plus a sized/sentinel model. See
[`06_ranges.md`](../06_ranges.md).

Practice the category printout:
[`examples/00_iterator_categories.cpp`](../examples/00_iterator_categories.cpp).
