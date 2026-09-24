# 01. Iterators: the language algorithms speak

An iterator is **a position in a range**, plus the operations that
position supports. It is not a pointer, but a pointer is the model
the random-access iterator copies.

```
  container:   [ A | B | C | D ]
  iterators:     ^           ^
               begin()      end()   (end is one-past-last, not a real element)

  range is the half-open interval  [begin, end)
```

Every STL algorithm is written against **the weakest iterator it
needs**. That is why `std::sort` will not take `list::iterator`:
sort needs to jump and to measure distances in O(1).

## The five categories (C++17 names)

```
  input          output
    \              /
     +--> forward
              |
              v
         bidirectional
              |
              v
         random access
              |
              v (C++20)
         contiguous     (vector, array, string, span)
```

| Category | You can | You cannot | Examples |
| --- | --- | --- | --- |
| Input | read once, `++` | write, multi-pass | `istream_iterator` |
| Output | write, `++` | read | `ostream_iterator`, raw insert iterators |
| Forward | read, multi-pass, `++` | `--`, jump | `forward_list`, `unordered_*` |
| Bidirectional | `++` and `--` | `it + n` in O(1) | `list`, `set`, `map` |
| Random access | `it+n`, `it[i]`, `a-b` | assume bytes touch | `deque` |
| Contiguous | plus `&*(it+n) == &*it + n` | — | `vector`, `array`, `string`, `span` |

Critical distinction: **`deque` is random-access but not
contiguous.** `d[i]` is O(1) (two hops: chunk map, then cell).
`std::copy` still works. A C API that wants a `T*` to the whole
range does **not**.

## Why `sort` rejects `list`

```
  sort's inner loop wants:
      jump to midpoint     it + n/2
      swap arbitrary       iter_swap(a, b)
      measure              last - first

  list iterator can only:
      ++it / --it          each step O(1), n steps O(n)
```

`list` has `sort()` as a **member**. That sort is merge-sort on
nodes: O(n log n) comparisons, no random access. Using
`std::sort(l.begin(), l.end())` is a type error, not a runtime
slowdown.

`std::lower_bound` on a `set` iterator is the opposite trap: it
**compiles**, does O(log n) comparisons, but **advances the
iterator O(n)** times. Always call `s.lower_bound(x)` (member).

```
  std::lower_bound(s.begin(), s.end(), x)     set::lower_bound(x)
  ---------------------------------------     -------------------
  binary search *thinks* it can jump          tree walk
  but ++ on a tree iterator is in-order       O(log n) pointer hops
  successor: O(1) amortized, still O(n) total
```

## `begin` / `end` and the empty range

```
  empty:   begin() == end()          there is no first element
  one:     begin() + 1 == end()      (if random access)
```

Never dereference `end()`. Algorithms return `end()` to mean
"not found". That is a position, not a boolean — you compare.

## Iterator adapters you will actually use

```
  container                adapter                         writes to
  [ a b c ]   <--- reverse_iterator ---  rbegin..rend      same elements, backward

  vector v;   back_inserter(v)     does push_back
  deque d;    front_inserter(d)    does push_front
  set s;      inserter(s, s.end()) does insert

  move_iterator            *it yields an rvalue     steal, do not copy
```

```cpp
std::vector<int> a{1, 2, 3}, b;
std::copy(a.begin(), a.end(), std::back_inserter(b));  // b grows
```

Without `back_inserter`, `std::copy` into `b.begin()` is undefined
if `b` is empty: copy **writes**, it does not insert.

## Invalidation, one picture

```
  vector:  insert/realloc     [ a b |c| d ]     every iterator after the
                                              hole (and all, if realloc) die

  list/set/map: erase(it)     only `it` dies;  other iterators still name
                                              their nodes

  unordered_map: rehash       ALL iterators die;  pointers to elements
                                              usually survive (node-based)
                                              but do not rely on it across
                                              a guaranteed rehash without
                                              reading your library docs
```

The LRU recipe stores `list` iterators inside an `unordered_map`.
That is legal **because** `list::erase` does not invalidate the
other list iterators, and the map only rehashes its **own**
iterators.

## C++20 ranges, in one box

```
  C++17:  std::sort(v.begin(), v.end());
  C++20:  std::ranges::sort(v);          // still iterators underneath
```

Ranges add *views* (`filter`, `transform`) that are lazy iterators.
The cost contract does not change: a filter view over a `list` is
still bidirectional, not random-access. This guide writes classic
iterator-pair code so the costs stay visible. Use `std::ranges::*`
at call sites when you have C++20; the algorithms chapter notes
equivalents.

## Practice

Run [`examples/00_iterator_categories.cpp`](examples/00_iterator_categories.cpp).
Adapters in depth: [`03_iterator_adapters.md`](03_iterator_adapters.md).
Ranges: [`06_ranges.md`](06_ranges.md).
