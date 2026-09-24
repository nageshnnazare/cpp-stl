// =============================================================================
// WHAT  : stack for matching parens; monotonic stack for next-greater.
// WHY   : LIFO only — one end. deque would work but documents the wrong shape.
//         No iterators: that is the point. Need to walk? Use vector + back().
// THREAD: push/pop/top from two threads without a mutex is a race. A parser
//         stack is almost always thread-local.
// ERASE : There is no erase-in-the-middle and no iteration. Delete = pop().
//         pop() returns void — save top() first. Cannot delete "the 7" buried
//         in the stack; that would be a vector or list.
// REMEMBER: monotonic stack stores indices, not values (you need the width).
//           Each index is pushed once and popped once → O(n).
// Guide : containers/stack.md, recipes/monotonic_stack_queue.md
// =============================================================================
#include <iostream>
#include <stack>
#include <string>
#include <vector>

bool valid(const std::string& s) {
    std::stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
            continue;
        }
        if (st.empty()) return false;
        char o = st.top();
        st.pop();
        if ((c == ')' && o != '(') || (c == ']' && o != '[') || (c == '}' && o != '{'))
            return false;
    }
    return st.empty();
}

std::vector<int> next_greater(const std::vector<int>& a) {
    int n = (int)a.size();
    std::vector<int> ans(n, -1);
    std::stack<int> st;  // indices of candidates to the right
    for (int i = n - 1; i >= 0; --i) {
        while (!st.empty() && a[st.top()] <= a[i]) st.pop();  // pop dominated
        if (!st.empty()) ans[i] = a[st.top()];
        st.push(i);
    }
    return ans;
}

int main() {
    std::cout << std::boolalpha;
    std::cout << "{[()]} " << valid("{[()]}") << "\n";
    std::cout << "{[(]}  " << valid("{[(]}") << "\n";

    auto ng = next_greater({2, 1, 2, 4, 3});
    std::cout << "next greater:";
    for (int x : ng) std::cout << " " << x;
    std::cout << "\n";
}
