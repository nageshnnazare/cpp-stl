// =============================================================================
// WHAT  : Singly linked list: insert_after / before_begin / erase_after.
// WHY   : One pointer per node. Almost always slower than vector; use when you
//         only walk forward and memory per node is the product constraint.
// THREAD: Same rule as list — const walk OK, any splice/insert/erase needs a
//         lock if shared. No operator[] so no "elem" concurrent index writes.
// ERASE : There is no erase(it). You erase AFTER a known iterator.
//         erase_after(before_begin()) == pop_front.
//         To delete while walking: keep `prev`, erase_after(prev) when
//         *next(prev) matches — do not ++ the iterator you just invalidated.
// REMEMBER: no size(), no --it, no push_back. before_begin() is the handle
//           for "insert at front via insert_after".
// Guide : containers/forward_list.md, 02_decision_guide.md
// =============================================================================
#include <forward_list>
#include <iostream>

int main() {
    std::forward_list<int> chain;
    chain.push_front(30);
    chain.push_front(10);

    auto it = chain.before_begin();
    ++it;  // now on 10 (the first real element)
    chain.insert_after(it, 20);  // 10, 20, 30

    std::cout << "chain:";
    for (int x : chain) std::cout << " " << x;  // read-only: OK
    std::cout << "\n";

    chain.erase_after(chain.before_begin());  // drop 10
    std::cout << "after pop_front via erase_after:";
    for (int x : chain) std::cout << " " << x;
    std::cout << "\n";

    std::cout << "no size(), no operator[], no --it\n";
}
