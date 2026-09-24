// =============================================================================
// WHAT  : reverse_iterator, back_inserter, move_iterator, ostream_iterator.
// WHY   : One copy algorithm, many destinations. Adapters make "grow a vector"
//         or "steal strings" look like an output iterator.
// THREAD: reverse walk is const if you don't assign through it. back_inserter
//         writes (push_back) — may realloc; don't hold other iterators into dest.
// ERASE : reverse_iterator.base() is one-past *rit. Erasing via reverse iters
//         is easy to get off-by-one — prefer a forward it = erase(it) loop.
// REMEMBER: copy into empty v.begin() is UB. back_inserter calls push_back.
//           After move_iterator construction, treat src as moved-from (clear it).
// Guide : 03_iterator_adapters.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4};
    std::cout << "reverse walk:";
    for (auto it = v.rbegin(); it != v.rend(); ++it) std::cout << " " << *it;
    std::cout << "\n";

    std::vector<int> evens;
    std::copy_if(v.begin(), v.end(), std::back_inserter(evens),
                 [](int x) { return x % 2 == 0; });
    std::cout << "copy_if + back_inserter:";
    for (int x : evens) std::cout << " " << x;
    std::cout << "\n";

    std::vector<std::string> src{"alpha", "beta", "gamma"};
    std::vector<std::string> dst(std::make_move_iterator(src.begin()),
                                 std::make_move_iterator(src.end()));
    std::cout << "moved dst[1]=" << dst[1] << "  src[1] empty-ish size=" << src[1].size()
              << "\n";

    std::cout << "ostream_iterator:";
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";
}
