// =============================================================================
// WHAT  : First unique: list of still-unique values + hash to list iterator + dead set.
// WHY   : show() is the oldest unique → list front. Second sighting must O(1)
//         cut that node → stored iterator. A queue alone cannot delete the
//         middle. dead_ so a third sighting is a no-op.
// THREAD: add writes. show is const. Lock if shared.
// ERASE : Second add: order_.erase(it) then pos_.erase then dead_.insert.
//         List erase does not invalidate other unique-value iterators.
//         Never range-for the list and erase inside — we erase by stored it.
// REMEMBER: unique values stay in insertion order. Duplicates are gone forever
//           (dead_). show() does not pop.
// Guide : recipes/first_unique.md, 03_list_lru.cpp (same iterator-stability)
// =============================================================================
#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>
#include <unordered_set>

class FirstUnique {
    std::list<int> order_;
    std::unordered_map<int, std::list<int>::iterator> pos_;
    std::unordered_set<int> dead_;

public:
    void add(int x) {
        if (dead_.count(x)) return;
        if (auto it = pos_.find(x); it != pos_.end()) {
            order_.erase(it->second);  // O(1); other uniques keep their iterators
            pos_.erase(it);
            dead_.insert(x);
            return;
        }
        order_.push_back(x);
        pos_[x] = std::prev(order_.end());
    }

    int show() const { return order_.empty() ? -1 : order_.front(); }
};

int main() {
    FirstUnique f;
    for (int x : {2, 3, 2, 6}) {
        f.add(x);
        std::cout << "add " << x << " first_unique=" << f.show() << "\n";
    }
}
