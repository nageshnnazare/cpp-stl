// =============================================================================
// WHAT  : First-hour STL: vector grow, range-for, unordered_map count, contains.
// WHY   : Beginners need one file that compiles and matches 00_beginner.md.
//         Default sequence = vector. Counting = ++m[k]. Membership = contains.
// THREAD: Local containers. ++freq[w] is a write — not safe to share.
// ERASE : Not shown. Do not for (x : v) v.erase. See 02_decision_guide.md.
// REMEMBER: v[i] on empty is UB. find returns end(), not a bool.
// Guide : 00_beginner.md, 14_read_a_problem.md, cheat_sheet.md
// =============================================================================
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
    std::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    std::cout << "size=" << v.size() << " first=" << v[0]
              << " last=" << v.back() << "\n";
    std::cout << "values:";
    for (int x : v) std::cout << " " << x;
    std::cout << "\n";

    std::unordered_map<std::string, int> freq;
    for (auto w : {"cat", "dog", "cat"}) ++freq[w];  // [] inserts 0, then ++
    std::cout << "cat=" << freq["cat"] << " dog=" << freq["dog"] << "\n";
    std::cout << std::boolalpha;
    std::cout << "contains bird=" << freq.contains("bird") << "\n";

    if (auto it = freq.find("dog"); it != freq.end())
        std::cout << "find dog -> " << it->second << "\n";
}
