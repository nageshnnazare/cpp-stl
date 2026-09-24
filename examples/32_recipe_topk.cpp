// =============================================================================
// WHAT  : Top-k frequent: count map + min-heap of size k, or frequency buckets.
// WHY   : Heap is O(n log k) and streaming-friendly. Buckets are O(n) when
//         frequency ≤ n (array index IS the count). Full sort of all uniques
//         is the slow default. Heap cannot "delete 7" — here we only pop min.
// THREAD: Local. Shared cnt: ++cnt[x] is a write. Merge thread-local counts.
// ERASE : Range-for over cnt is read-only (structured bindings copy). Heap:
//         pop when size > k — no iterators. Bucket: walk from high freq down,
//         no erase.
// REMEMBER: pair in the heap is {count, value} so greater<> compares count first.
//           bucket[c] is a vector of values that appeared c times.
// Guide : recipes/top_k_frequent.md, 13_priority_queue_topk.cpp
// =============================================================================
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

std::vector<int> top_k_heap(const std::vector<int>& a, int k) {
    std::unordered_map<int, int> cnt;
    for (int x : a) ++cnt[x];
    using P = std::pair<int, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> h;  // min-heap of {freq, val}
    for (auto [val, c] : cnt) {
        h.push({c, val});
        if ((int)h.size() > k) h.pop();
    }
    std::vector<int> out;
    while (!h.empty()) {
        out.push_back(h.top().second);
        h.pop();
    }
    return out;
}

std::vector<int> top_k_bucket(const std::vector<int>& a, int k) {
    std::unordered_map<int, int> cnt;
    for (int x : a) ++cnt[x];
    std::vector<std::vector<int>> bucket(a.size() + 1);
    for (auto [val, c] : cnt) bucket[c].push_back(val);
    std::vector<int> out;
    for (int f = (int)bucket.size() - 1; f >= 1 && (int)out.size() < k; --f)
        for (int v : bucket[f]) {
            out.push_back(v);
            if ((int)out.size() == k) return out;
        }
    return out;
}

int main() {
    std::vector<int> a{1, 1, 1, 2, 2, 3};
    auto h = top_k_heap(a, 2);
    auto b = top_k_bucket(a, 2);
    std::cout << "heap:";
    for (int x : h) std::cout << " " << x;
    std::cout << "\nbucket:";
    for (int x : b) std::cout << " " << x;
    std::cout << "\n";
}
