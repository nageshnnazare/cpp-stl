// =============================================================================
// WHAT  : Daily temperatures — next greater to the right via a monotonic stack.
// WHY   : Each index is pushed once and popped once → O(n). Nested "for j>i"
//         is O(n²). Stack holds *indices* so wait[i] = j - i is a subtraction.
// THREAD: Thread-local stack. The temperatures vector is read-only here.
// ERASE : Delete = pop a dominated index. No middle erase. No range-for on stack.
//         Cannot "delete 71 from the stack" without popping everything above it —
//         that would be the wrong container.
// REMEMBER: while top is smaller than today, it just found its next greater.
//           Leftovers at the end have wait 0 (no warmer day).
// Guide : recipes/monotonic_stack_queue.md, 11_stack_parens_next_greater.cpp
// =============================================================================
#include <iostream>
#include <stack>
#include <vector>

std::vector<int> daily_temperatures(const std::vector<int>& t) {
    int n = (int)t.size();
    std::vector<int> wait(n, 0);
    std::stack<int> st;  // indices of unresolved (not-yet-warmer) days
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && t[st.top()] < t[i]) {
            wait[st.top()] = i - st.top();
            st.pop();
        }
        st.push(i);
    }
    return wait;
}

int main() {
    auto w = daily_temperatures({73, 74, 75, 71, 69, 72, 76, 73});
    std::cout << "days to wait:";
    for (int x : w) std::cout << " " << x;
    std::cout << "\n";  // 1 1 4 2 1 1 0 0
}
