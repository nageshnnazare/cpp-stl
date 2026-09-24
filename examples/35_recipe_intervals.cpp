// =============================================================================
// WHAT  : Merge overlapping intervals (sort + scan) + max concurrent (sweep map).
// WHY   : Merge needs order of starts — vector + sort, not unordered_map.
//         Max rooms: map<time, delta> so we walk time in order. A heap of
//         end-times also works; hash would scramble the sweep.
// THREAD: Local. If intervals arrive from many threads, merge into one vector
//         under a lock, then sort once — do not sort concurrently in place.
// ERASE : Merge writes out, never erases from iv during the walk (would be
//         O(n²) and invalidate). Sweep walks delta read-only; no erase.
// REMEMBER: sort by start. Overlap is next.start <= cur.end (closed intervals).
//           ++delta[start], --delta[end]; walk keys in order; track running sum.
// Guide : recipes/intervals.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

std::vector<std::pair<int, int>> merge_intervals(std::vector<std::pair<int, int>> iv) {
    if (iv.empty()) return {};
    std::sort(iv.begin(), iv.end());
    std::vector<std::pair<int, int>> out{iv[0]};
    for (std::size_t i = 1; i < iv.size(); ++i) {
        if (iv[i].first <= out.back().second)
            out.back().second = std::max(out.back().second, iv[i].second);  // extend, don't erase
        else
            out.push_back(iv[i]);
    }
    return out;
}

int max_concurrent(const std::vector<std::pair<int, int>>& iv) {
    std::map<int, int> delta;
    for (auto [s, e] : iv) {
        ++delta[s];
        --delta[e];
    }
    int cur = 0, best = 0;
    for (auto [t, d] : delta) {  // keys in time order — why map, not unordered_map
        (void)t;
        cur += d;
        best = std::max(best, cur);
    }
    return best;
}

int main() {
    auto m = merge_intervals({{1, 3}, {2, 6}, {8, 10}, {15, 18}});
    std::cout << "merged:";
    for (auto [s, e] : m) std::cout << " [" << s << "," << e << "]";
    std::cout << "\n";

    std::vector<std::pair<int, int>> meetings{{0, 30}, {5, 10}, {15, 20}};
    std::cout << "meeting rooms needed: " << max_concurrent(meetings) << "\n";
}
