// =============================================================================
// WHAT  : iota, accumulate (init type!), prefix sums, adjacent_difference, inner_product.
// WHY   : Prefix sums turn range-sum into O(1) after O(n) prep. The *type of
//         the 0* is the result type — 0 is int, 0LL is long long (overflow).
// THREAD: Writes the output range. Input can be a concurrent const walk.
// ERASE : None. partial_sum writes dest; dest may alias src (in-place OK).
//         Do not erase src while accumulating.
// REMEMBER: pref[0]=0, pref[i]=sum of first i. sum[L..R) = pref[R]-pref[L].
//           adjacent_difference[0] is a[0] itself, then deltas.
// Guide : algorithms/numeric.md
// =============================================================================
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> idx(5);
    std::iota(idx.begin(), idx.end(), 0);
    std::cout << "iota:";
    for (int x : idx) std::cout << " " << x;
    std::cout << "\n";

    std::vector<int> a{2, 3, 1, 5};
    std::cout << "accumulate with 0   (int): " << std::accumulate(a.begin(), a.end(), 0)
              << "\n";
    std::cout << "accumulate with 0LL: " << std::accumulate(a.begin(), a.end(), 0LL) << "\n";

    std::vector<int> pref(a.size() + 1);
    std::partial_sum(a.begin(), a.end(), pref.begin() + 1);  // pref[0] stays 0
    std::cout << "prefix:";
    for (int x : pref) std::cout << " " << x;
    std::cout << "  sum[1..3]=" << (pref[4] - pref[1]) << "\n";

    std::vector<int> d(a.size());
    std::adjacent_difference(a.begin(), a.end(), d.begin());
    std::cout << "adjacent_difference:";
    for (int x : d) std::cout << " " << x;
    std::cout << "\n";

    std::vector<int> b{4, 5, 6};
    std::cout << "inner_product a·b (first 3 of a): "
              << std::inner_product(a.begin(), a.begin() + 3, b.begin(), 0) << "\n";
}
