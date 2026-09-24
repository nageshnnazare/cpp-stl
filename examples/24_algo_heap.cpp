// =============================================================================
// WHAT  : make_heap / push_heap / pop_heap / sort_heap on a plain vector.
// WHY   : priority_queue is this with the vector hidden. Use heap algorithms
//         when you need to peek at the storage, or interleave other ops.
//         Cannot erase an arbitrary 7 — that is a multiset (or lazy tombstones).
// THREAD: All heap ops write the range. Not thread-safe.
// ERASE : pop_heap swaps max to back, then you pop_back. That is the only
//         legal "delete from a heap". There is no erase-while-walk: the
//         vector is not sorted, and walking it is not heap-order.
// REMEMBER: push: push_back then push_heap. pop: pop_heap then pop_back.
//           sort_heap destroys the heap (leaves sorted ascending).
// Guide : algorithms/heap.md, containers/priority_queue.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <vector>

void show(const char* label, const std::vector<int>& h) {
    std::cout << label;
    for (int x : h) std::cout << " " << x;
    std::cout << "  is_heap=" << std::boolalpha
              << std::is_heap(h.begin(), h.end()) << "\n";
}

int main() {
    std::vector<int> h{3, 1, 4, 1, 5};
    std::make_heap(h.begin(), h.end());
    show("after make_heap", h);

    h.push_back(9);
    std::push_heap(h.begin(), h.end());  // sift the new back() up
    show("after push_heap", h);

    std::pop_heap(h.begin(), h.end());  // max now at back
    int mx = h.back();
    h.pop_back();
    std::cout << "popped " << mx << "\n";
    show("after pop_heap", h);

    std::sort_heap(h.begin(), h.end());
    show("after sort_heap (not a heap)", h);
}
