// =============================================================================
// WHAT  : set = unique + sorted + predecessor. multiset erase-one vs erase-all.
// WHY   : Need min/max and successor of 4 while inserts arrive. unordered_set
//         has no lower_bound. Sorted vector insert in the middle is O(n).
// THREAD: Concurrent contains/find OK. Any insert/erase needs a mutex if shared.
//         Walking begin() while another thread inserts is a data race.
// ERASE : only the erased iterator dies. Legal: it = s.erase(it).
//         Range-for + erase is still UB (the loop's it is the dead one).
//         multiset::erase(key) wipes EVERY equal key. erase(find(x)) wipes one.
// REMEMBER: *it is const Key — change a key with extract, not *it =.
//           Use member lower_bound, not std::lower_bound on the iterators.
// Guide : containers/set.md, 02_decision_guide.md, 04_insert_emplace_extract.md
// =============================================================================
#include <iostream>
#include <iterator>
#include <set>

int main() {
    std::set<int> ids;
    for (int x : {5, 1, 9, 5, 3, 7}) ids.insert(x);  // duplicate 5 is ignored
    std::cout << "unique sorted:";
    for (int x : ids) std::cout << " " << x;  // in-order walk
    std::cout << "\nmin=" << *ids.begin() << " max=" << *std::prev(ids.end()) << "\n";
    std::cout << "successor of 4: " << *ids.lower_bound(4) << "\n";

    std::multiset<int> bag{3, 1, 3, 2, 3};
    std::cout << "multiset count(3)=" << bag.count(3) << "\n";
    bag.erase(bag.find(3));  // ONE 3 — this is the "delete-anywhere heap" move
    std::cout << "after erase(find(3)) count=" << bag.count(3) << "\n";
    bag.erase(3);  // ALL remaining 3s — classic bug if you meant one
    std::cout << "after erase(3) count=" << bag.count(3) << "\n";
}
