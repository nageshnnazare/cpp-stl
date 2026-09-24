// =============================================================================
// WHAT  : BFS shortest path on an unweighted graph: queue + dist vector.
// WHY   : FIFO = increasing distance. Weighted graphs need a priority_queue
//         (Dijkstra), not this. Graph storage: vector<vector<int>> because
//         ids are dense 0..n-1 (not unordered_map).
// THREAD: queue is not thread-safe. Parallel BFS is a different algorithm
//         (partition the frontier) with its own synchronization.
// ERASE : queue has no iterators. Delete = pop() the front only.
//         dist[] is a vector: we only write each node once (first visit).
// REMEMBER: pop() returns void. Check dist[v] < 0 for "unseen", not a set,
//           when ids are 0..n-1.
// Guide : containers/queue.md, recipes/graph_bfs_dijkstra.md
// =============================================================================
#include <iostream>
#include <queue>
#include <vector>

std::vector<int> bfs(const std::vector<std::vector<int>>& g, int src) {
    int n = (int)g.size();
    std::vector<int> dist(n, -1);
    std::queue<int> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : g[u]) {  // read-only walk of neighbors
            if (dist[v] >= 0) continue;
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }
    return dist;
}

int main() {
    std::vector<std::vector<int>> g(4);
    g[0] = {1, 2};
    g[1] = {0, 3};
    g[2] = {0};
    g[3] = {1};
    auto d = bfs(g, 0);
    for (int i = 0; i < 4; ++i)
        std::cout << "dist[0->" << i << "]=" << d[i] << "\n";
}
