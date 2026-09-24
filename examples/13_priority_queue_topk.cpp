// =============================================================================
// WHAT  : top-k via a min-heap of size k. Default priority_queue is a max-heap.
// WHY   : Stream / k << n. If all n values are already in a vector, nth_element
//         or partial_sort is simpler. Heap cannot erase an arbitrary value —
//         that is a multiset (or lazy tombstones).
// THREAD: push/pop/top are writes/reads of the same structure — lock if shared.
// ERASE : No iterators, no erase(7). Delete the extreme with pop() only.
//         Cannot range-for a priority_queue. Drain by pop into a vector.
// REMEMBER: min-heap = priority_queue<T, vector<T>, greater<T>>.
//           top() is O(1), push/pop O(log n).
// Guide : containers/priority_queue.md, recipes/top_k_frequent.md
// =============================================================================
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

std::vector<int> top_k(const std::vector<int>& a, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> h;  // min-heap
    for (int x : a) {
        h.push(x);
        if ((int)h.size() > k) h.pop();  // drop the smallest of the winners
    }
    std::vector<int> out;
    while (!h.empty()) {
        out.push_back(h.top());
        h.pop();
    }
    return out;  // ascending
}

int main() {
    auto k3 = top_k({5, 1, 9, 3, 7, 8}, 3);
    std::cout << "top 3:";
    for (int x : k3) std::cout << " " << x;
    std::cout << "\n";

    std::priority_queue<int> maxh;  // default: largest on top
    for (int x : {3, 1, 5}) maxh.push(x);
    std::cout << "max-heap top=" << maxh.top() << "\n";
}
