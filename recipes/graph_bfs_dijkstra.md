# Graphs — `vector` of lists, then `queue` or `priority_queue`

A graph is already two containers: **nodes → neighbors**.
Search is a third: BFS queue, DFS stack, Dijkstra heap.

```
  dense ids 0..n-1                 sparse / string ids
  vector<vector<int>> g(n);        unordered_map<T, vector<T>> g;

  0 --1-- 1 --4-- 2
   \      |
    2     3
     \    |
      --- 2

  g[0] = {1, 2}
  g[1] = {2}
  g[2] = {}
```

**Why not `map<int, vector<int>>` for 0..n-1?** The key *is*
the index. **Why not `list` for neighbors?** You scan neighbors;
`vector` is faster. **Why `vector<vector<int>>` not a matrix?**
Matrix is O(n²) space; use it when you need O(1) "is edge u,v?"
and n is small.

## Unweighted shortest: `queue` + `vector<int> dist`

```
  queue: frontier in increasing distance
  dist[v] = -1 unseen

  this IS BFS
```

Related: number of islands (grid = implicit graph, `queue` of
pairs or flatten), word ladder (`unordered_set` dictionary +
BFS), rotting oranges.

## Weighted shortest: `priority_queue` + `vector<long long> dist`

```
  lazy Dijkstra:

  pq  (dist, node)   min-heap
  pop d,u
  if d != dist[u]: stale, continue
  for each edge u->v,w:
      if dist[u]+w < dist[v]:
          dist[v] = dist[u]+w
          push (dist[v], v)
```

```
  true decrease-key Dijkstra:

  set<pair<long long,int>> st;     // ordered by dist
  when relaxing, st.erase({old, v}); st.insert({new, v});
```

**Why lazy heap in interviews?** Less code, same O((n+m) log m)
with duplicates. **Why `set`?** O((n+m) log n) without extras,
erase needs the old dist. **Why not `queue`?** Wrong on
positive weights. **Why not DFS?** Not shortest.

0-1 weights: **deque** (0 to front, 1 to back).

## Topological sort: Kahn = `queue` + `vector<int> indegree`

```
  indegree[v] = how many incoming
  queue all indegree==0
  pop u, for v in g[u]: if --indegree[v]==0 push v
  if you popped < n: cycle
```

DFS topo uses a `stack` (recursion or explicit) + 3-color
`vector`. Same `g`.

## Related problems

- Number of Islands, Clone Graph, Course Schedule I/II
- Word Ladder I/II (BFS + parent `unordered_map`)
- Network Delay Time (Dijkstra)
- Cheapest flights within k stops (BFS/Bellman/Dijkstra+stops)
- Alien dictionary (graph of letters + topo)
- Reconstruct itinerary (Eulerian: `map` of `multiset` edges,
  Hierholzer stack)

Run [`examples/36_recipe_graph.cpp`](../examples/36_recipe_graph.cpp).
