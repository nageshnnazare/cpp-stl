// =============================================================================
// WHAT  : Min-stack: values stack + a second stack of minima.
// WHY   : A single stack cannot answer "min of everything below" in O(1).
//         Parallel mins_ only stores a new min when x <= current min, so
//         duplicates of the min are preserved (pop must pop mins too).
//         One vector of pairs {val, min_so_far} is the same idea.
// THREAD: push/pop write both stacks. get_min is const vs other const, race
//         vs push. Typical use is thread-local.
// ERASE : Delete = pop both if the popped value *is* the min. No iteration.
//         Cannot delete a buried 5 — that would be a vector.
// REMEMBER: push min on <= (not <) so two equal mins both sit on mins_.
//           pop mins_ only when vals_.top() == mins_.top().
// Guide : recipes/min_stack.md, containers/stack.md
// =============================================================================
#include <iostream>
#include <stack>

class MinStack {
    std::stack<int> vals_;
    std::stack<int> mins_;

public:
    void push(int x) {
        vals_.push(x);
        if (mins_.empty() || x <= mins_.top()) mins_.push(x);
    }

    void pop() {
        if (vals_.top() == mins_.top()) mins_.pop();
        vals_.pop();
    }

    int top() const { return vals_.top(); }
    int get_min() const { return mins_.top(); }
};

int main() {
    MinStack s;
    for (int x : {3, 5, 2, 2, 7}) {
        s.push(x);
        std::cout << "push " << x << " min=" << s.get_min() << " top=" << s.top() << "\n";
    }
    s.pop();
    std::cout << "pop 7 min=" << s.get_min() << "\n";
    s.pop();
    std::cout << "pop 2 min=" << s.get_min() << " (still 2, duplicate min)\n";
    s.pop();
    std::cout << "pop 2 min=" << s.get_min() << " (expect 3)\n";
}
