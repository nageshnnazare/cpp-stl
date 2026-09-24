# `std::priority_queue` — always the extreme, O(log n) to change it

A binary heap on top of a `vector`. Default: **max-heap**
(`less<T>`: the "largest" sits at `top()`).

```
  push 3, 1, 5, 4

       5
      / \
     4   3
    /
   1

  top() == 5     pop -> 4 sits on top
```

Header: `<queue>` (same header as `queue`). **No iterators, no
decrease-key, no erase-by-value.**

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `push`, `emplace` | O(log n) |
| R | `top`, `empty`, `size` | O(1) |
| U | none (cannot mutate `top` into a smaller key safely) | — |
| D | `pop` | O(log n) |

Min-heap:

```cpp
std::priority_queue<int, std::vector<int>, std::greater<int>> lo;
```

Custom: wrap a `struct` with `operator<`, or pass a functor.

## Job 1 — top-k

Stream of n scores, keep the k best.

```
  k=3   incoming 5, 1, 9, 3, 7, 8

  min-heap of size k (the *smallest of the winners* at top):
  [5]  [1,5]  [1,5,9]  3<1? no  7: pop 1 push 7 -> [5,7,9]  8: pop 5 -> [7,8,9]
```

A min-heap of size k is O(n log k). Sorting everything is
O(n log n). `partial_sort` / `nth_element` win when **all n
values are already in a vector**.

**Why not `multiset`?** It can, O(n log k) too, and supports
erase. The heap is less memory and the usual answer.

## Job 2 — Dijkstra / "smallest tentative first"

```
  pq:  (dist, node)
  pop the smallest dist; skip if stale (lazy delete)

  graph  0 --1-- 1 --4-- 2
          \--4-- 2

  push (0,0)
  pop 0; push (1,1), (4,2)
  pop 1; push (1+4,2)=(5,2)   5>4, stale later
  pop 4 at node 2            done
```

There is no `decrease_key` on `priority_queue`. Push another
pair; when a worse pair surfaces, compare to `best[u]` and
continue. See [`recipes/graph_bfs_dijkstra.md`](../recipes/graph_bfs_dijkstra.md).

**Why not `queue`?** FIFO is not "smallest dist". That is BFS,
wrong on weighted graphs. **Why not `set<pair<dist,node>>`?**
You *can* erase the old pair (true decrease-key). Slightly more
code, sometimes better on dense graphs. Interview default: lazy
heap.

## Job 3 — merge k sorted lists

k heads; heap of (value, which-list, iterator). Each pop, push
the next from that list. O(N log k).

```
  lists:  1-4-5     1-3-4     2-6
  heap tops: 1,1,2
  pop 1 (list0) -> emit 1, push 4
  pop 1 (list1) -> emit 1, push 3
  ...
```

## Job 4 — scheduling / "next event"

CPU tasks, Huffman coding, swimming in rising water: the next
thing that happens is the min key. That sentence is a heap.

## Why this, not that

| Alternative | Prefer heap | Prefer the other |
| --- | --- | --- |
| `sort` whole array | k << n or infinite stream | you need full order |
| `nth_element` | stream / you already pop over time | one-shot k-th in a vector |
| `multiset` | only pop extreme | need erase(value) or min **and** max |
| `set<pair>` Dijkstra | lazy + simple | true decrease-key |

## Related problems

- Kth Largest (215), Top K Frequent (347), K Closest Points
- Merge k Sorted Lists (23)
- Dijkstra, Prim, Huffman
- Meeting rooms II (min-heap of end times) — or a sorted delta
  map
- Find median from stream (two heaps)
- Reorganize string / task scheduler (heap of frequencies)

Run [`examples/13_priority_queue_topk.cpp`](../examples/13_priority_queue_topk.cpp).
