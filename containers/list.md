# `std::list` — O(1) splice if you already hold the node

A `list` is a **doubly linked** chain. The only operations that
are cheaper than `vector` are: insert/erase **at a known
iterator**, and `splice` (move a node to another list or position
without copying `T`).

```
  sentinel
     |
     +--> [p| 7 |n] <--> [p| 3 |n] <--> [p| 9 |n] --+
     ^                                               |
     +-----------------------------------------------+

  erase(it_to_3):  relink 7 <-> 9      O(1)
                   other iterators still valid
```

Header: `<list>`. Iterator: **bidirectional**. No `[]`.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `push_front/back`, `insert(it, x)`, `emplace`, `splice` | O(1) at `it` |
| R | `front`, `back`; walk | O(1) ends; k-th is O(k) |
| U | `*it =` | O(1) |
| D | `erase(it)`, `pop_*`, `remove`, `remove_if`, `unique` | `erase` O(1); `remove` O(n) |
| extra | `sort`, `merge`, `reverse` | member `sort` is O(n log n) mergesort |

`size()` is O(1) (C++11). `splice(pos, other, it)` is O(1) and
does not invalidate `it`.

## Job 1 — LRU order (the reason `list` still exists)

You need "move this entry to most-recent" in O(1). A vector would
shift. A list can `splice` the node to `begin()`.

```
  most recent                                                 least
  [ D ] <--> [ B ] <--> [ A ] <--> [ C ]
    ^
    hash["B"] stores the iterator to the B node
    get("B"):  splice that node to front
```

The hash table is a second container. Full design:
[`recipes/lru_cache.md`](../recipes/lru_cache.md),
[`examples/03_list_lru.cpp`](../examples/03_list_lru.cpp).

**Why not `deque`?** Erasing a node in the *middle* of a deque is
O(n) and invalidates iterators you stored in the hash. **Why not
`vector`?** Same. **Why not only `unordered_map`?** A map has no
"order of use"; you would scan all entries to find the LRU.

## Job 2 — playlist / editor buffer with held cursors

A music queue or a text piece-table: many cursors, cut this
range, paste it elsewhere. `splice` moves the nodes.

```
  list A:  [a] [b] [c] [d]
  splice b..c into list B at pos:
  A:  [a] [d]
  B:  [x] [b] [c] [y]
  no copy of the song metadata
```

If `T` is an `int`, this is slower than copying ints in a vector.
`splice` wins when `T` is fat or identity must be stable
(iterators held by the UI).

## Why this is usually the wrong default

```
  "I insert in the middle a lot"
        |
        +--> Do you already have an iterator to that middle?
                no: you walk O(n) to find it. vector insert is
                    also O(n), but the n is sequential moves.
                yes: list insert is O(1). this is the LRU case.
```

Scanning a `list` of 10⁶ ints can be **10–50×** slower than a
`vector` scan. Do not "optimize" a vector into a list without a
held-iterator story.

## Related problems

- LRU Cache (146), LFU Cache (460) — LFU adds a frequency map
- Design a browser history that splices a branch on new navigate
- Reverse nodes in k-group (linked-list problems; you may still
  *implement* with `std::list` to practice `splice`)
- First unique number in a stream (queue + map of iterators;
  `list` can drop a value from the middle when it becomes
  duplicate)

Run [`examples/03_list_lru.cpp`](../examples/03_list_lru.cpp).
