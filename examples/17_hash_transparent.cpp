// =============================================================================
// WHAT  : Custom hash for pair<int,int>; transparent set<string, less<>>
//         so find(string_view) does not allocate a temporary string.
// WHY   : No std::hash<pair>. If hash(a)!=hash(b) when a==b, find misses.
//         Hot path of URL/symbol lookup: skip SSO/heap on every find.
// THREAD: contains is const — concurrent contains OK. insert is a write.
// ERASE : not shown. set: it = s.erase(it). unordered_set: same, but insert
//         during a walk may rehash.
// REMEMBER: equal keys MUST hash equal. Mix both fields of a pair.
//           contains("be") is exact match, not prefix; lower_bound is prefix-ish.
// Guide : 05_hash_compare_lookup.md
// =============================================================================
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        std::size_t h1 = std::hash<int>{}(p.first);
        std::size_t h2 = std::hash<int>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));  // mix; not h1 alone
    }
};

int main() {
    std::unordered_set<std::pair<int, int>, PairHash> seen;
    seen.insert({1, 2});
    std::cout << "seen (1,2)=" << std::boolalpha << seen.contains({1, 2})
              << " (3,4)=" << seen.contains({3, 4}) << "\n";

    std::set<std::string, std::less<>> words;  // less<> is transparent
    words.insert("alpha");
    words.insert("beta");
    std::string_view q{"alpha"};
    std::cout << "set.find(string_view) hit=" << (words.find(q) != words.end()) << "\n";
    std::cout << "contains(\"be\")=" << words.contains("be") << " (exact key, not prefix)\n";
    auto it = words.lower_bound(std::string_view{"b"});
    std::cout << "lower_bound(\"b\")=" << *it << "\n";
}
