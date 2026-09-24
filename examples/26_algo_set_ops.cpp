// =============================================================================
// WHAT  : merge, set_intersection/union/difference, includes — on *sorted* ranges.
// WHY   : Linear in |A|+|B|. Faster than putting both into a set and walking.
//         Unsorted input → garbage (not a diagnostic). Need a live set? use set.
// THREAD: Read A and B, write the destination. Destination is exclusive.
//         A and B may be shared const across threads.
// ERASE : These write a *new* range via back_inserter. They do not erase A/B.
//         To "delete from A everything in B": set_difference into a new vector,
//         then swap. Do not erase A while scanning B.
// REMEMBER: Both ranges must be sorted the same way. includes is subset test.
//           intersection of multisets keeps min multiplicity.
// Guide : algorithms/set_operations.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

void show(const char* label, const std::vector<int>& v) {
    std::cout << label;
    for (int x : v) std::cout << " " << x;
    std::cout << "\n";
}

int main() {
    std::vector<int> A{1, 3, 3, 5, 8};
    std::vector<int> B{3, 4, 8, 9};

    std::vector<int> merged;
    std::merge(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(merged));
    show("merge", merged);

    std::vector<int> inter;
    std::set_intersection(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(inter));
    show("intersection", inter);

    std::vector<int> uni;
    std::set_union(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(uni));
    show("union", uni);

    std::vector<int> diff;
    std::set_difference(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(diff));
    show("A-B", diff);

    std::vector<int> needle{3, 8};  // named so begin/end are the same object
    std::cout << "includes {3,8} in A? " << std::boolalpha
              << std::includes(A.begin(), A.end(), needle.begin(), needle.end()) << "\n";
}
