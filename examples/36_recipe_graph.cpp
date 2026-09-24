// =============================================================================
// WHAT  : Unweighted BFS (queue) vs weighted Dijkstra (min-heap of {dist, node}).
// WHY   : Dense 0..n-1 ids → vector of adj lists, not map. Unweighted shortest
//         path is FIFO. Weighted needs "smallest dist first" = priority_queue.
//         Lazy Dijkstra: push duplicates, skip if d != dist[u] (no decrease-key).
// THREAD: dist[] writes are the algorithm. Parallel Dijkstra is a different
//         paper. Do not share pq across threads.
// ERASE : queue/pq: pop only. dist[v] written once (BFS) or when improved
//         (Dijkstra). Neighbor walk is read-only — do not erase edges while walking.
// REMEMBER: pop then skip stale heap entries. inf = max/4 to avoid overflow on +.
// Guide : recipes/graph_bfs_dijkstra.md, 12_queue_bfs.cpp
// =============================================================================
#include <iostream>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

std::vector<int> bfs(const std::vector<std::vector<int>>& g, int src) {
    std::vector<int> dist(g.size(), -1);
    std::queue<int> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : g[u])
            if (dist[v] < 0) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
    }
    return dist;
}

std::vector<long long> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& g, int src) {
    const long long inf = std::numeric_limits<long long>::max() / 4;
    std::vector<long long> dist(g.size(), inf);
    using P = std::pair<long long, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d != dist[u]) continue;  // stale entry after a better path
        for (auto [v, w] : g[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

int main() {
    std::vector<std::vector<int>> unweighted = {{1, 2}, {0, 3}, {0}, {1}};
    auto b = bfs(unweighted, 0);
    std::cout << "BFS dist:";
    for (int x : b) std::cout << " " << x;
    std::cout << "\n";

    std::vector<std::vector<std::pair<int, int>>> wg(3);
    wg[0] = {{1, 1}, {2, 4}};
    wg[1] = {{2, 4}};
    auto d = dijkstra(wg, 0);
    std::cout << "Dijkstra 0->2: " << d[2] << " (expect 4, not 1+4=5)\n";
}
