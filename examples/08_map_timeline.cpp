// =============================================================================
// WHAT  : map as time→price predecessor, and as a sweep-line of +1/−1 deltas.
// WHY   : Need successor/predecessor of a time that may not be stored.
//         unordered_map has no order. vector<pair>+sort if all writes precede
//         queries; map if they interleave.
// THREAD: price_at takes const map& — concurrent calls OK if nobody writes.
//         ++delta[s] is a write (and [] may insert). Lock around the build.
// ERASE : not shown. it = m.erase(it) is legal. Do not mutate it->first.
//         it->second = is a legal in-place update during a walk.
// REMEMBER: m[k] INSERTS a default V if k is new. find/contains for membership.
//           upper_bound(t) is first key > t; --it is last key <= t if it exists.
// Guide : containers/map.md, recipes/merge_intervals.md, 07_pitfalls.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

int price_at(const std::map<int, int>& snaps, int t) {
    auto it = snaps.upper_bound(t);  // first timestamp strictly after t
    if (it == snaps.begin()) return -1;  // nothing at or before t
    return std::prev(it)->second;
}

int max_overlap(const std::vector<std::pair<int, int>>& iv) {
    std::map<int, int> delta;  // key order IS the algorithm
    for (auto [s, e] : iv) {
        ++delta[s];  // [] default 0 then increment — Create-or-Update on purpose
        --delta[e];
    }
    int cur = 0, best = 0;
    for (auto [t, d] : delta) {  // walk times in sorted order
        cur += d;
        best = std::max(best, cur);
        (void)t;
    }
    return best;
}

int main() {
    std::map<int, int> snaps{{10, 5}, {20, 8}, {40, 3}};
    std::cout << "t=25 -> " << price_at(snaps, 25) << " (expect 8)\n";
    std::cout << "t=5  -> " << price_at(snaps, 5) << " (expect -1)\n";
    std::cout << "t=40 -> " << price_at(snaps, 40) << " (expect 3)\n";

    std::vector<std::pair<int, int>> meetings{{0, 30}, {5, 10}, {15, 20}};
    std::cout << "max concurrent meetings: " << max_overlap(meetings) << "\n";

    std::map<std::string, int> m;
    std::cout << "m[\"x\"]=" << m["x"] << " size now " << m.size() << " (inserted 0)\n";
}
