# `std::deque` — O(1) at both ends, still indexable

A `deque` (double-ended queue) is a **map of fixed-size chunks**.
You get `d[i]` in O(1) and `push_front` / `push_back` without
moving the other elements.

```
           chunk map (small vector of pointers)
           +-----+-----+-----+-----+
           |  *  |  *  |  *  |  *  |
           +--|--+--|--+--|--+--|--+
              |     |     |     |
              v     v     v     v
           [__ab] [cdef] [ghij] [kl__]
             ^                     ^
           front                 back

  d[0] is 'a'   d[5] is 'f'   index = chunk# + offset
```

Header: `<deque>`. Iterator: **random access, not contiguous**.
No `data()`.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `push_front`, `push_back`, `emplace_*`, `insert` | ends A O(1); middle O(n) |
| R | `[]`, `at`, `front`, `back` | O(1) (worse constant than vector) |
| U | `d[i]=` | O(1) |
| D | `pop_front`, `pop_back`, `erase`, `clear` | ends O(1); middle O(n) |

Pointers / references to **elements** stay valid when you push/pop
the ends. Iterators do **not**. Never hold a `deque::iterator`
across an end-insert.

## Job 1 — sliding window of the last k samples

```
  stream:  1  3  -1  -3  5  3  6  7     k=3
  window:  [1 3 -1] [-1 -3 5] ...
  deque stores indices, values decreasing:

  i=0  dq: [0]           max=1
  i=1  dq: [1]           3≥1, pop 0
  i=2  dq: [1 2]         3, -1
  i=3  dq: [1 2 3] -> drop 1 (out of window) -> [2 3]
```

Why a deque, not a `queue`? You pop the **back** when a new value
is bigger (those older values can never be the window max) *and*
pop the **front** when it slides out of range. A `queue` cannot
touch the back. A `vector` pop_front is O(n).

Full walkthrough: [`recipes/sliding_window.md`](../recipes/sliding_window.md)
and [`examples/02_deque_sliding_window.cpp`](../examples/02_deque_sliding_window.cpp).

## Job 2 — BFS when you also inject at the front

0-1 BFS (edges of weight 0 or 1): push weight-0 neighbors to the
**front**, weight-1 to the **back**. That is a deque, not a
`queue`.

```
  deque:  front [ 0-edges | 1-edges ] back
          pop_front always yields current smallest tentative dist
          among the 0/1 frontier
```

Plain BFS: `std::queue` is clearer (it *is* a deque underneath).

## Job 3 — a work list that retries at the front

A parser, a task runner, or "re-queue failed job next":
`push_front(job)`. `vector` would shift everyone.

## Why this, not that

| Alternative | Why people reach for it | Why deque instead (or not) |
| --- | --- | --- |
| `vector` + `insert(begin)` | "dynamic array" | Each front insert is O(n). n pushes = O(n²) |
| `list` | "cheap ends" | True, but you lose `d[i]` and cache |
| `queue` | BFS | Use `queue` if you never `push_front` / index |
| `vector` as ring buffer | hot path, known cap | Faster if you manage head/tail yourself; more code |

**Do not** use `deque` as your default array. Chunk hopping
hurts `sort`, `find`, and anything that streams linearly. Prefer
`vector` unless an **end other than the back** is in the hot loop.

## Related problems

- Sliding Window Maximum (LeetCode 239)
- 0-1 BFS / deque Dijkstra on 0/1 weights
- Design a circular deque (641)
- Palindrome checks with pop front+back (or two indices on a string)
- Moving average from data stream (deque of last k + a running sum)

Run [`examples/02_deque_sliding_window.cpp`](../examples/02_deque_sliding_window.cpp).
