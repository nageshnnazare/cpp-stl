// =============================================================================
// WHAT  : Running median: max-heap of the lower half + min-heap of the upper half.
// WHY   : Need the middle, not a full sort per insert. Two heaps give O(log n)
//         add and O(1) median. Need to *delete* an arbitrary value later?
//         Heaps cannot — use two multisets instead (see recipes/median_stream).
// THREAD: add writes both heaps. median is const *if* no other thread adds.
//         Shared MedianFinder needs one mutex around add+median.
// ERASE : Only pop the extreme after a rebalance. No walk, no erase(value).
//         To drop a stale number: lazy tombstone set, skip on top(), or multisets.
// REMEMBER: lo.size() == hi.size() or lo is one bigger. Always push then rebalance.
// Guide : recipes/median_stream.md, containers/priority_queue.md
// =============================================================================
#include <iostream>
#include <queue>
#include <vector>

class MedianFinder {
    std::priority_queue<int> lo;                                          // max-heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> hi;    // min-heap

public:
    void add(int x) {
        if (lo.empty() || x <= lo.top())
            lo.push(x);
        else
            hi.push(x);
        if ((int)lo.size() > (int)hi.size() + 1) {
            hi.push(lo.top());
            lo.pop();
        } else if (hi.size() > lo.size()) {
            lo.push(hi.top());
            hi.pop();
        }
    }

    double median() const {
        if (lo.size() > hi.size()) return lo.top();
        return (lo.top() + hi.top()) / 2.0;
    }
};

int main() {
    MedianFinder mf;
    for (int x : {1, 5, 2, 8, 3}) {
        mf.add(x);
        std::cout << "after " << x << " median=" << mf.median() << "\n";
    }
}
