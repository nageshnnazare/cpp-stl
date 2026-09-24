// =============================================================================
// WHAT  : Two Sum from ASK / ARRIVE / EXTRA — blanks stay n/a.
// WHY   : The statement never mentions delete, update-by-id, or key order.
//         You do not fill seven CRUD blanks. You underline three facts.
// THREAD: Local map. Two threads on last[] = data race.
// ERASE : Delete is n/a in the spec. We only insert into the map.
// REMEMBER: ASK = two indices. EXTRA = none. → unordered_map, not map, not list.
// Guide : 14_read_a_problem.md (sparse + decode 1)
// =============================================================================
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

// "Given an array and a target, return two indices that sum to target."
//
//   ASK     two indices
//   ARRIVE  whole array, one query
//   EXTRA   none
//
//   delete  n/a
//   order   n/a   →  not map
//   by-id   n/a   →  the "id" is just the scan index
//
// Hot op inferred from ASK: have I seen target-a[i]?  →  unordered_map

std::pair<int, int> two_sum(const std::vector<int>& a, int target) {
    std::unordered_map<int, int> last;
    for (int i = 0; i < (int)a.size(); ++i) {
        if (auto it = last.find(target - a[i]); it != last.end())
            return {it->second, i};
        last[a[i]] = i;
    }
    return {-1, -1};
}

int main() {
    auto [i, j] = two_sum({2, 7, 11, 15}, 9);
    std::cout << "indices " << i << " " << j << " (expect 0 1)\n";
    auto [p, q] = two_sum({3, 2, 4}, 6);
    std::cout << "indices " << p << " " << q << " (expect 1 2)\n";
}
