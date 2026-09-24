// =============================================================================
// WHAT  : Longest substring without repeat (last-seen array) + sliding-window max (deque).
// WHY   : Unique-chars: left pointer only moves forward; array[256] is denser
//         than unordered_map for bytes. Window max: deque of decreasing indices
//         so front is always the max. Heap would need lazy delete of expired ids.
// THREAD: Local. The deque is not a concurrent queue.
// ERASE : deque: pop_front expired, pop_back dominated — ends only, never
//         erase in the middle while walking. last[] overwrites; no container erase.
// REMEMBER: store indices in the deque, not values (you need to know expiry).
//           front() is max; back() is the newest candidate.
// Guide : recipes/sliding_window.md, 02_deque_sliding_window.cpp
// =============================================================================
#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

int length_of_longest_unique(const std::string& s) {
    std::array<int, 256> last{};
    last.fill(-1);
    int left = 0, best = 0;
    for (int i = 0; i < (int)s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (last[c] >= left) left = last[c] + 1;  // shrink until c is unique
        last[c] = i;
        best = std::max(best, i - left + 1);
    }
    return best;
}

std::vector<int> window_max(const std::vector<int>& a, int k) {
    std::deque<int> dq;  // indices, a[dq] strictly decreasing
    std::vector<int> out;
    for (int i = 0; i < (int)a.size(); ++i) {
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();  // expired
        if (i >= k - 1) out.push_back(a[dq.front()]);
    }
    return out;
}

int main() {
    std::cout << "abcabcbb -> " << length_of_longest_unique("abcabcbb") << "\n";
    std::cout << "bbbbb -> " << length_of_longest_unique("bbbbb") << "\n";
    auto mx = window_max({1, 3, -1, -3, 5, 3, 6, 7}, 3);
    std::cout << "window max:";
    for (int x : mx) std::cout << " " << x;
    std::cout << "\n";
}
