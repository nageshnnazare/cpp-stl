// =============================================================================
// WHAT  : Print each container's iterator strength, and the set lower_bound trap.
// WHY   : Algorithms speak iterators, not containers. sort needs random-access;
//         list only has ++/-- so std::sort(list) will not compile.
// THREAD: All three containers here are local to main — no sharing.
//         Concurrent const walks of a vector/list/set would be OK; any insert
//         from another thread is a data race.
// ERASE : Not shown. Remember: range-for + erase is UB on every container.
//         vector: erase invalidates it and everything after.
//         list/set: only the erased iterator dies — `it = c.erase(it)`.
// REMEMBER: vector it - begin() is O(1). list distance is O(n).
//           ALWAYS s.lower_bound(x), never std::lower_bound(s.begin(), s.end(), x).
// Guide : 01_iterators.md, algorithms/iterator_requirements.md, 02_decision_guide.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <vector>

template <class It>
const char* strength() {
    // C++20 concepts look at iterator_concept, not the legacy iterator_category
    // (vector's *category* is still random_access for ABI). Prefer concepts.
    if constexpr (std::contiguous_iterator<It>)
        return "contiguous";  // bytes touch: vector, array, string, span
    else if constexpr (std::random_access_iterator<It>)
        return "random_access";  // deque: it+n is O(1) but NOT contiguous
    else if constexpr (std::bidirectional_iterator<It>)
        return "bidirectional";  // list, set, map: ++ and --
    else if constexpr (std::forward_iterator<It>)
        return "forward";  // forward_list, unordered_*
    else
        return "weaker";
}

int main() {
    std::vector<int> v{1, 3, 5, 7, 9};
    std::list<int> l{1, 3, 5, 7, 9};
    std::set<int> s{1, 3, 5, 7, 9};

    std::cout << "vector: " << strength<decltype(v.begin())>() << "\n";
    std::cout << "list:   " << strength<decltype(l.begin())>() << "\n";
    std::cout << "set:    " << strength<decltype(s.begin())>() << "\n";

    // Random-access: iterator subtraction is O(1). This is why sort works.
    std::cout << "vector index of 7: " << (std::find(v.begin(), v.end(), 7) - v.begin())
              << "  (subtraction O(1))\n";

    // Bidirectional: std::distance walks ++ n times.
    auto lit = std::find(l.begin(), l.end(), 7);
    std::cout << "list distance to 7: " << std::distance(l.begin(), lit)
              << "  (O(n) increments)\n";

    // Member: O(log n) tree hops. THIS is the one to call on set/map.
    auto m = s.lower_bound(6);
    std::cout << "set::lower_bound(6) = " << *m << "\n";
    // Algorithm: O(log n) *comparisons* but O(n) ++ on a tree iterator. Compiles!
    auto a = std::lower_bound(s.begin(), s.end(), 6);
    std::cout << "std::lower_bound(set, 6) = " << *a
              << "  (same value; O(n) hops — use the member)\n";

    // copy WRITES; empty out.begin() would be UB. back_inserter calls push_back.
    std::vector<int> out;
    std::copy(v.begin(), v.end(), std::back_inserter(out));
    std::cout << "back_inserter copied " << out.size() << " elements\n";
}
