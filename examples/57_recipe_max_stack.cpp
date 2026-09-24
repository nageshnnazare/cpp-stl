// =============================================================================
// WHAT  : Max-stack: list as the stack + map<value, vector of list iterators>.
// WHY   : Need pop (LIFO) AND pop_max (delete the most recent occurrence of
//         the max, even if it is buried). A parallel max-stack like MinStack
//         cannot delete a buried max. list.erase(stored it) is O(1) and does
//         not invalidate the other stored iterators.
// THREAD: push/pop/pop_max write both. peek_max is const. Lock if shared.
// ERASE : pop_max: st_.erase(at_[x].back()) then pop that handle. pop: pop_back
//         the list and the vector of handles. If the vector empties, map.erase
//         the key. Never range-for the list and erase.
// REMEMBER: at_[x] is a stack of iterators so the last push of x is .back().
//           peek_max is rbegin of the map (largest key).
// Guide : recipes/max_stack.md, 44_recipe_min_stack.cpp (contrast)
// =============================================================================
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>

class MaxStack {
    std::list<int> st_;
    std::map<int, std::vector<std::list<int>::iterator>> at_;

public:
    void push(int x) {
        st_.push_back(x);
        at_[x].push_back(std::prev(st_.end()));
    }

    int pop() {
        int x = st_.back();
        st_.pop_back();
        at_[x].pop_back();
        if (at_[x].empty()) at_.erase(x);
        return x;
    }

    int top() const { return st_.back(); }
    int peek_max() const { return at_.rbegin()->first; }

    int pop_max() {
        int x = peek_max();
        st_.erase(at_[x].back());  // may be in the middle; other iterators stay
        at_[x].pop_back();
        if (at_[x].empty()) at_.erase(x);
        return x;
    }
};

int main() {
    MaxStack s;
    for (int x : {3, 8, 5, 8}) s.push(x);
    std::cout << "top=" << s.top() << " max=" << s.peek_max() << "\n";
    std::cout << "popMax=" << s.pop_max() << " top=" << s.top() << " max=" << s.peek_max()
              << "\n";
    std::cout << "pop=" << s.pop() << " max=" << s.peek_max() << "\n";
    std::cout << "popMax=" << s.pop_max() << " top=" << s.top() << "\n";
}
