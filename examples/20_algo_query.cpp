// =============================================================================
// WHAT  : Non-modifying algorithms: find, count, any_of/all_of, search, adjacent_find.
// WHY   : Read-only scans. Prefer these over a handwritten for-loop when the
//         name documents intent. They do not allocate and do not invalidate.
// THREAD: Const walk of a vector another thread is not writing = OK.
//         Concurrent push_back on v while find runs = data race (and maybe realloc).
// ERASE : These do not erase. They return iterators into the live range.
//         Do not erase(*it) through a range-for. After you erase, the returned
//         iterator from find is dead on vector (and everything after it).
// REMEMBER: find is O(n) linear. Sorted? lower_bound. Hash? unordered_map::find.
//           it - begin() is O(1) only for random-access (vector/string/array).
// Guide : algorithms/non_modifying.md, 02_decision_guide.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};

    auto it = std::find(v.begin(), v.end(), 5);  // first 5, or end()
    std::cout << "find 5 -> index " << (it - v.begin()) << "\n";
    std::cout << "count of 1: " << std::count(v.begin(), v.end(), 1) << "\n";
    std::cout << "any even: " << std::boolalpha
              << std::any_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; }) << "\n";
    std::cout << "all < 10: " << std::all_of(v.begin(), v.end(), [](int x) { return x < 10; })
              << "\n";

    std::string hay = "ababcababd";
    std::string needle = "cab";
    auto p = std::search(hay.begin(), hay.end(), needle.begin(), needle.end());
    std::cout << "search cab at " << (p - hay.begin()) << "\n";

    auto adj = std::adjacent_find(v.begin(), v.end());  // first pair of equal neighbors
    std::cout << "adjacent_find equal pair? " << (adj == v.end() ? "none" : "found") << "\n";
}
