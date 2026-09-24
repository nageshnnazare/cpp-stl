# Non-modifying algorithms — look, do not shuffle

These walk `[first, last)` and leave elements where they are.
Header: `<algorithm>`. Time O(n) unless noted. Extra space O(1).

```
  [ 3 | 1 | 4 | 1 | 5 | 9 ]
    ^                   ^
  first               last     (last is one-past)

  find(4) stops here ----^
  count(1) == 2
  any_of(even) == true
```

## What each one answers

| Algorithm | Question | Returns |
| --- | --- | --- |
| `find` / `find_if` / `find_if_not` | where is the first match? | iterator or `last` |
| `find_end` | last occurrence of a subsequence | iterator |
| `find_first_of` | first element that is in another set | iterator |
| `adjacent_find` | first pair of equal (or pred) neighbors | iterator |
| `count` / `count_if` | how many? | `difference_type` |
| `mismatch` / `equal` | where do two ranges diverge? / are they equal? | pair of iterators / bool |
| `search` / `search_n` | where is this subsequence / n copies? | iterator |
| `all_of` / `any_of` / `none_of` | predicate on the whole range? | bool |
| `for_each` | do `f` on each (side effects) | `f` (moved) |
| `for_each_n` (C++17) | first n | iterator past n |

C++20: `std::ranges::find(v, x)` — same cost, less `begin/end`.

## Job 1 — replace a handwritten scan

```cpp
// before
int i = 0;
for (; i < (int)v.size(); ++i)
    if (v[i] == x) break;

// after
auto it = std::find(v.begin(), v.end(), x);
if (it == v.end()) { /* missing */ }
```

**Why the algorithm?** Intent is one word. Off-by-one on `i`
disappears. For `list` you cannot write `v[i]` anyway; `find`
still works.

**Why not `unordered_set`?** If you search **once**, a scan is
O(n) and has no hash build. If you search **many** times, pay
for a set.

## Job 2 — validation as `all_of`

```cpp
bool ok = std::all_of(rows.begin(), rows.end(),
                      [](const Row& r){ return r.qty >= 0; });
```

`any_of` short-circuits on the first true; `all_of` on the first
false. A `for` with `return` is the same machine — use the
algorithm when the predicate is the point of the line.

## Job 3 — `search` vs `string::find`

On `string`, `s.find(sub)` is the specialized tool (and may use
a better string algorithm). `std::search` is the generic version
for any forward iterators (vector of tokens, list of events).

```
  hay:   [ A B C A B D ]
  needle:[ A B D ]
  search -> position of last three
```

## Critical thinking

| Instinct | Better when |
| --- | --- |
| `for_each` to fill a new vector | `transform` / `copy_if` — those *name* the output |
| `count_if` then `find_if` | one `find_if` if you only need existence |
| `equal` on unsorted vs sorted | `equal` is positional; for set-equality `sort` both or dump into `unordered_set` |
| `find` on a **sorted** vector | `binary_search` / `lower_bound` — O(log n) |

`mismatch` is the implementation of "where did two strings
diverge" and of `equal` (no mismatch).

## Related problems

- First unique character (count in `array<int,26>`, then `find_if`)
- Is subsequence (`search` is the wrong primitive — two pointers)
- Repeated substring (`s+s` find, or KMP via `search` with a
  custom searcher in C++17)
- Check if array is pairwise consecutive (`adjacent_find` with
  a pred)

Run [`examples/20_algo_query.cpp`](../examples/20_algo_query.cpp).
