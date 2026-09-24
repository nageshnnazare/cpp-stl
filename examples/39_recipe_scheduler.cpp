// =============================================================================
// WHAT  : Reorganize string: max-heap of remaining counts + one-slot cooldown.
// WHY   : Always place the currently most frequent remaining letter, then
//         hold the previous letter out of the heap for one turn (so it cannot
//         sit next to itself). Greedy. Impossible leftover → "".
// THREAD: Local heap + count map.
// ERASE : Heap pop is the only delete. Count map is filled then drained into
//         the heap — we do not erase while iterating cnt (structured binding
//         copies; then we only push).
// REMEMBER: prev sits *outside* the heap for one pop. Push it back if count>0.
//           If prev still has count when the heap is empty, adjacent repeats
//           were unavoidable.
// Guide : recipes/scheduler.md, containers/priority_queue.md
// =============================================================================
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

std::string reorganize(const std::string& s) {
    std::unordered_map<char, int> cnt;
    for (char c : s) ++cnt[c];
    using P = std::pair<int, char>;
    std::priority_queue<P> h;  // max-heap of {remaining, char}
    for (auto [c, n] : cnt) h.push({n, c});

    std::string out;
    P prev{0, '#'};  // last placed letter, cooling for one turn
    while (!h.empty()) {
        auto [n, c] = h.top();
        h.pop();
        out.push_back(c);
        if (prev.first > 0) h.push(prev);
        prev = {n - 1, c};
    }
    if (prev.first > 0) return "";  // leftover means impossible
    return out;
}

int main() {
    for (auto s : {"aab", "aaab", "vvvlo"})
        std::cout << s << " -> [" << reorganize(s) << "]\n";
}
