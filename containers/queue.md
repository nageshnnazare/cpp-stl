# `std::queue` — FIFO

Adapter over `deque` by default. `push` back, `pop` front.

```
  front                             back
    |                                 |
    v                                 v
  [ start ] [ a ] [ b ] [ c ] [ newly pushed ]
    ^
   pop
```

Header: `<queue>`. No iterators. `front()` and `back()` exist;
`priority_queue` has only `top()`.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `push`, `emplace` | O(1) |
| R | `front`, `back`, `empty`, `size` | O(1) |
| U | `front() =` | O(1) |
| D | `pop` | O(1) |

## Job 1 — BFS / level order

```
  graph:  0 -- 1 -- 3
           \-- 2

  queue:  [0]
          [1, 2]      dist[1]=1, dist[2]=1
          [2, 3]      ...
          [3]
          []
```

This is the shortest path on an **unweighted** graph. Weighted:
[`priority_queue`](priority_queue.md) / Dijkstra.

```cpp
std::queue<int> q;
q.push(src);
std::vector<int> dist(n, -1);
dist[src] = 0;
while (!q.empty()) {
    int u = q.front(); q.pop();
    for (int v : g[u])
        if (dist[v] < 0) { dist[v] = dist[u] + 1; q.push(v); }
}
```

## Job 2 — sliding window of *values* (not max)

Moving average: keep the last k, plus a running sum.

```
  k=3   stream 1, 10, 3, 5
  q:[1]         avg 1
  q:[1,10]      avg 5.5
  q:[1,10,3]    avg 14/3
  pop 1, push 5  q:[10,3,5]  avg 6
```

If you need the **max** in the window, a plain queue is not
enough — you need a deque (monotonic) or a `multiset`.

## Job 3 — producer / consumer in one thread

A job list: workers `push`, a loop `pop`. In multithreaded code
you still need a mutex; `queue` is not thread-safe.

## Why this, not that

| Alternative | Use queue | Use the other |
| --- | --- | --- |
| `deque` | you want the word "queue" | you `push_front` or index |
| `list` | — | almost never for BFS |
| `priority_queue` | unweighted / FIFO fairness | "smallest key first" |
| `vector` + index head | micro-optimized BFS | extra code |

## Related problems

- Binary tree level order (102), zigzag (103) (zigzag: deque)
- Number of islands / rotting oranges / walls and gates
- Word Ladder (127) — BFS on words + `unordered_set`
- Course Schedule (Kahn's algorithm: queue of indegree 0)
- Open the Lock (752), Shortest Path in Binary Matrix

Run [`examples/12_queue_bfs.cpp`](../examples/12_queue_bfs.cpp).
