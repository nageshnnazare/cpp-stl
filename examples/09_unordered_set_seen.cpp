// =============================================================================
// WHAT  : unordered_set as a seen-set; longest consecutive run via hash lookup.
// WHY   : Membership only — no order. insert returns {it, bool}; the bool IS
//         the test (do not count then insert — two hashes).
// THREAD: insert is a write (may rehash → all iterators die). Concurrent
//         insert from two threads is UB. contains from many threads is OK
//         if nobody inserts.
// ERASE : erase(it) invalidates only it, returns next (C++11). insert MAY
//         rehash and kill EVERY iterator — do not insert while walking.
//         longest_consecutive only reads `s` in the loop: OK.
// REMEMBER: Dense 0..n ids → vector<char> visited, not a hash set.
//           Worst-case find is O(n) (one bucket). reserve before a flood.
// Guide : containers/unordered_set.md, 02_decision_guide.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

int longest_consecutive(const std::vector<int>& a) {
    std::unordered_set<int> s(a.begin(), a.end());
    int best = 0;
    // Read-only walk. We never insert/erase here, so iterators stay valid.
    for (int x : s) {
        if (s.count(x - 1)) continue;  // not a run start
        int y = x, len = 0;
        while (s.count(y)) {
            ++y;
            ++len;
        }
        best = std::max(best, len);
    }
    return best;
}

int main() {
    std::unordered_set<int> seen;
    for (int v : {3, 1, 3, 7, 1}) {
        auto [it, ok] = seen.insert(v);  // one hash: inserted?
        std::cout << "insert " << v << (ok ? " new\n" : " duplicate\n");
        (void)it;
    }

    std::vector<int> a{100, 4, 200, 1, 3, 2};
    std::cout << "longest consecutive: " << longest_consecutive(a) << "\n";
}
