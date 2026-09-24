// =============================================================================
// WHAT  : lower_bound / upper_bound / binary_search on a *sorted* vector.
// WHY   : O(log n) only if the range is ordered with the same comparator.
//         Unsorted vector → wrong answer, not a compile error. Need insert
//         by key often? map. Need membership only? unordered_set.
// THREAD: Const lookups OK concurrently. insert at lower_bound is a write
//         (shifts the tail) — exclusive. Do not hold `lo` across a push_back.
// ERASE : Not shown. To drop a value: auto [a,b] = equal_range(...); v.erase(a,b);
//         That invalidates iterators from a onward — finish the search first.
// REMEMBER: lower_bound = first >= x. upper_bound = first > x. count = hi - lo.
//           ALWAYS container.lower_bound on set/map, never std::lower_bound.
// Guide : algorithms/binary_search.md, 07_pitfalls.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{1, 3, 3, 7, 9, 12};

    auto lo = std::lower_bound(v.begin(), v.end(), 3);
    auto hi = std::upper_bound(v.begin(), v.end(), 3);
    std::cout << "lower_bound(3) index " << (lo - v.begin()) << "\n";
    std::cout << "upper_bound(3) index " << (hi - v.begin()) << "\n";
    std::cout << "count of 3: " << (hi - lo) << "\n";
    std::cout << "binary_search(4): " << std::boolalpha
              << std::binary_search(v.begin(), v.end(), 4) << "\n";

    auto ins = std::lower_bound(v.begin(), v.end(), 4);
    std::cout << "insert 4 at index " << (ins - v.begin()) << " to keep sorted\n";
    v.insert(ins, 4);  // may realloc; all prior iterators die

    int L = 3, R = 9;
    auto a = std::lower_bound(v.begin(), v.end(), L);
    auto b = std::upper_bound(v.begin(), v.end(), R);
    std::cout << "how many in [" << L << "," << R << "]: " << (b - a) << "\n";
}
