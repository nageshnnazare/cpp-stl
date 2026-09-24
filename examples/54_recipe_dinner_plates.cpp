// =============================================================================
// WHAT  : Dinner plates: vector of stacks + set of indices that still have room.
// WHY   : push goes to the leftmost non-full stack. pop_at(i) needs random
//         access → vector. "Leftmost hole" → set.begin(). Scanning the vector
//         each push would be O(n).
// THREAD: push/pop write both. Not thread-safe.
// ERASE : pop_at pops one plate then holes_.insert(i). trim pops empty stacks
//         from the right and holes_.erase(that index). set.erase by key, not
//         while iterating holes_. Stack itself: pop only.
// REMEMBER: holes_ stores indices with size < cap, including empties in the
//           middle. Leftmost push is *holes_.begin().
// Guide : recipes/dinner_plates.md
// =============================================================================
#include <iostream>
#include <set>
#include <stack>
#include <vector>

class DinnerPlates {
    int cap_;
    std::vector<std::stack<int>> st_;
    std::set<int> holes_;  // indices with size < cap_

    void trim() {
        while (!st_.empty() && st_.back().empty()) {
            holes_.erase((int)st_.size() - 1);
            st_.pop_back();
        }
    }

public:
    explicit DinnerPlates(int cap) : cap_(cap) {}

    void push(int val) {
        if (holes_.empty()) {
            st_.emplace_back();
            holes_.insert((int)st_.size() - 1);
        }
        int i = *holes_.begin();  // leftmost non-full
        st_[i].push(val);
        if ((int)st_[i].size() == cap_) holes_.erase(i);
    }

    int pop_at(int i) {
        if (i < 0 || i >= (int)st_.size() || st_[i].empty()) return -1;
        int v = st_[i].top();
        st_[i].pop();
        holes_.insert(i);
        return v;
    }

    int pop() {
        trim();
        if (st_.empty()) return -1;
        int v = pop_at((int)st_.size() - 1);
        trim();
        return v;
    }
};

int main() {
    DinnerPlates d(2);
    for (int x : {1, 2, 3, 4, 5}) d.push(x);
    std::cout << "popAt(0)=" << d.pop_at(0) << " (expect 2)\n";
    d.push(20);
    d.push(21);
    std::cout << "pop=" << d.pop() << " pop=" << d.pop() << " pop=" << d.pop()
              << " (expect 21 5 4)\n";
}
