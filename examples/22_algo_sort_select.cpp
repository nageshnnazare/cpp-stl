// =============================================================================
// WHAT  : sort vs nth_element vs partial_sort vs stable_sort.
// WHY   : Full sort is O(n log n) and orders everything. If you only need
//         "the k-th" (median, top-k cutoff), nth_element is O(n) average.
//         Need the k smallest *in order*? partial_sort. Ties must keep
//         enrollment order? stable_sort (not sort).
// THREAD: In-place writes. Do not share the vector with another thread.
// ERASE : None of these erase. They permute. After sort, binary search is legal.
//         After nth_element, only v[k] is the k-th; sides are unordered — do
//         not binary_search the whole vector.
// REMEMBER: nth_element(begin, begin+k, end) → v[k] is in sorted position.
//           partial_sort sorts [begin, middle); the rest is unspecified.
// Guide : algorithms/sorting.md, 02_decision_guide.md (algorithm picker)
// =============================================================================
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::vector<int> a{7, 1, 5, 3, 9, 2};
    auto full = a;
    std::sort(full.begin(), full.end());
    std::cout << "sort:";
    for (int x : full) std::cout << " " << x;
    std::cout << "\n";

    auto nth = a;
    std::nth_element(nth.begin(), nth.begin() + 3, nth.end());
    std::cout << "nth_element k=3 -> v[3]=" << nth[3] << "  (left<=, right>=, unsorted sides):";
    for (int x : nth) std::cout << " " << x;
    std::cout << "\n";

    auto part = a;
    std::partial_sort(part.begin(), part.begin() + 3, part.end());
    std::cout << "partial_sort k=3 prefix:";
    for (int i = 0; i < 3; ++i) std::cout << " " << part[i];
    std::cout << "\n";

    using P = std::pair<int, std::string>;
    std::vector<P> rows{{2, "Ann"}, {1, "Bob"}, {2, "Cyd"}, {1, "Dan"}};
    std::stable_sort(rows.begin(), rows.end(),
                     [](const P& x, const P& y) { return x.first < y.first; });
    std::cout << "stable_sort by grade (ties keep enrollment order):\n";
    for (auto& [g, name] : rows) std::cout << "  " << g << " " << name << "\n";
}
