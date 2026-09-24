// =============================================================================
// WHAT  : vector as a timeline (append + index), prefix sums, erase-remove.
// WHY   : Default sequence. O(1) index, amortized O(1) push_back, cache-hot.
//         Not list: you need log[i]. Not deque: you never push the front.
// THREAD: Concurrent reads of different v[i] are OK if nobody reallocates.
//         push_back from another thread while you hold log[3] is a data race
//         AND may dangle the reference after realloc.
// ERASE : Do NOT for (auto x : mixed) mixed.erase(...). That is UB.
//         remove_if compact keepers left; erase the tail in one shot (O(n)).
//         A loop of mixed.erase(it) is O(n²) shifts.
// REMEMBER: reserve before a known flood. resize constructs; reserve does not.
//           clear() keeps capacity. iterator/reference dies on realloc.
// Guide : containers/vector.md, 02_decision_guide.md, 09_memory_reserve_pmr.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    struct Tick {
        int t;
        int x;
    };
    std::vector<Tick> log;
    log.reserve(8);  // capacity >= 8, size still 0 — do not log[0]= yet
    for (int i = 0; i < 5; ++i)
        log.push_back({i, (i + 1) * (i + 1)});  // 1,4,9,16,25

    std::cout << "timeline size=" << log.size() << " cap=" << log.capacity() << "\n";
    std::cout << "tick[3].x = " << log[3].x << "\n";  // O(1) index — why not list

    std::vector<int> a;
    for (auto& tk : log) a.push_back(tk.x);  // range-for is READ-only here: OK

    // pref[0]=0, pref[i] = sum of first i elements. Range sum is O(1) after O(n).
    std::vector<int> pref(a.size() + 1);
    std::partial_sum(a.begin(), a.end(), pref.begin() + 1);
    int l = 1, r = 3;
    std::cout << "sum[" << l << ".." << r << "] = " << (pref[r + 1] - pref[l]) << "\n";

    std::vector<int> mixed{3, -1, 4, -2, 5, 0, -7};
    // Legal delete-while-"iterating": the algorithm walks, then we chop the tail.
    mixed.erase(std::remove_if(mixed.begin(), mixed.end(),
                               [](int x) { return x <= 0; }),
                mixed.end());
    std::cout << "positives:";
    for (int x : mixed) std::cout << " " << x;
    std::cout << "\n";
}
