// =============================================================================
// WHAT  : string_view split (no copy) + sliding unique-char window.
// WHY   : string owns; string_view is a (ptr,len) into it. find is the text API.
//         Window "have I seen this char?" is unordered_set / array<last index>.
// THREAD: Views do not own. If another thread mutates `line` (+= that reallocs),
//         every string_view into it dangles — data race + use-after-free.
// ERASE : longest_unique erases from `seen` while r walks forward — that is a
//         hash-set erase(key), O(1) avg, no iterator held. Fine.
//         Do not erase from `seen` via a range-for on `seen` itself.
// REMEMBER: substr on a view does not copy. Returning views of a local string
//           is dangling. acc = acc + piece is O(n²); += amortizes. reserve.
// Guide : containers/string.md, containers/string_view.md, 07_pitfalls.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

std::vector<std::string_view> split(std::string_view s, char delim) {
    std::vector<std::string_view> out;
    for (std::size_t i = 0; i < s.size();) {
        auto j = s.find(delim, i);
        if (j == std::string_view::npos) j = s.size();
        out.emplace_back(s.substr(i, j - i));  // view, not a new string
        i = j + (j < s.size() ? 1 : 0);
        if (j == s.size()) break;
    }
    return out;  // callers must keep the owner of s alive
}

int longest_unique(std::string_view s) {
    std::unordered_set<char> seen;
    int left = 0, best = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        // Shrink from the left until s[r] is unique in [left, r].
        while (seen.count(s[r])) {
            seen.erase(s[left]);  // erase by key, not by iterator from a walk of seen
            ++left;
        }
        seen.insert(s[r]);
        best = std::max(best, r - left + 1);
    }
    return best;
}

int main() {
    std::string line = "cat,dog,bird";
    std::cout << "tokens:";
    for (auto t : split(line, ',')) std::cout << " [" << t << "]";
    std::cout << "\n";

    std::cout << "longest unique in abcabcbb: " << longest_unique("abcabcbb") << "\n";

    std::string acc;
    acc.reserve(16);
    acc += "hello";  // not acc = acc + "hello" (that may allocate every time)
    acc += ", ";
    acc += "world";
    std::cout << acc << "\n";
}
