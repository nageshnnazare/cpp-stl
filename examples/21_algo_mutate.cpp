// =============================================================================
// WHAT  : transform, copy_if, erase-remove, unique, rotate — the mutating family.
// WHY   : erase-remove is how you delete from a vector. unique only squeezes
//         *adjacent* dups — sort first if you want a real set of values.
// THREAD: All of these write the range. Exclusive ownership. back_inserter
//         may realloc the destination — do not hold iterators into dest.
// ERASE : erase(remove_if(...), end) is THE legal bulk delete on vector.
//         remove_if only *slides* keepers left; the tail is junk until erase.
//         unique is the same two-step. Never for (x : v) v.erase(...).
// REMEMBER: copy_if into empty.begin() is UB — use back_inserter.
//           rotate(first, new_first, last): new_first becomes the front.
// Guide : algorithms/modifying.md, 02_decision_guide.md (pattern 1)
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
    std::vector<int> a{1, 2, 3, 4, 5};
    std::vector<int> doubled;
    std::transform(a.begin(), a.end(), std::back_inserter(doubled),
                   [](int x) { return 2 * x; });
    show("doubled", doubled);

    std::vector<int> odds;
    std::copy_if(a.begin(), a.end(), std::back_inserter(odds),
                 [](int x) { return x % 2; });
    show("odds", odds);

    std::vector<int> mixed{3, -1, 4, 0, -2, 5};
    mixed.erase(std::remove_if(mixed.begin(), mixed.end(), [](int x) { return x <= 0; }),
                mixed.end());  // compact then chop the junk tail
    show("erase-remove <=0", mixed);

    std::vector<int> dups{1, 1, 2, 2, 2, 3, 1};
    auto d2 = dups;
    d2.erase(std::unique(d2.begin(), d2.end()), d2.end());  // 3 1 still both present
    show("unique without sort", d2);
    std::sort(dups.begin(), dups.end());
    dups.erase(std::unique(dups.begin(), dups.end()), dups.end());
    show("sort+unique", dups);

    std::vector<char> rot{'A', 'B', 'C', 'D', 'E'};
    std::rotate(rot.begin(), rot.begin() + 2, rot.end());  // C D E A B
    std::cout << "rotate left 2: ";
    for (char c : rot) std::cout << c;
    std::cout << "\n";
}
