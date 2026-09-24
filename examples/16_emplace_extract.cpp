// =============================================================================
// WHAT  : ++m[k] vs try_emplace vs insert_or_assign; extract to rename a key;
//         merge two maps without copying values.
// WHY   : [] default-constructs V — Fat has no default ctor, so [] would not
//         compile. try_emplace builds V only when the key is new.
// THREAD: extract unlinks a node (write). merge writes both maps. Not safe to
//         share either map across threads without a lock.
// ERASE : extract(k) is delete-from-tree + you still own T. insert(move(nh))
//         is create. merge moves unique keys; duplicates stay in the source
//         (no silent overwrite).
// REMEMBER: insert({k,v}) does not overwrite. insert_or_assign does.
//           Failed emplace of an rvalue key may consume the key — try_emplace.
// Guide : 04_insert_emplace_extract.md, 07_pitfalls.md
// =============================================================================
#include <iostream>
#include <map>
#include <string>
#include <utility>

struct Fat {
    std::string blob;
    explicit Fat(std::string s) : blob(std::move(s)) {}  // no Fat() — [] would fail
};

int main() {
    std::map<int, int> cnt;
    ++cnt[7];  // membership+count: [] inserting 0 is the feature
    std::cout << "count[7]=" << cnt[7] << "\n";

    std::map<std::string, Fat> m;
    m.try_emplace("a", "first");
    auto [it, ok] = m.try_emplace("a", "ignored because key exists");
    std::cout << "second try_emplace inserted=" << std::boolalpha << ok
              << " blob=" << it->second.blob << "\n";

    m.insert_or_assign("a", Fat{"replaced"});
    std::cout << "after insert_or_assign blob=" << m.at("a").blob << "\n";

    std::map<int, std::string> names{{1, "ada"}, {2, "grace"}};
    auto nh = names.extract(1);  // 1 is gone from names; string not destroyed
    nh.key() = 10;               // mutate key on the handle (not through *it)
    names.insert(std::move(nh));
    std::cout << "renamed 1->10: " << names.at(10) << " has 1? " << names.contains(1)
              << "\n";

    std::map<int, std::string> extra{{2, "dup"}, {3, "al"}};
    names.merge(extra);  // 3 moves; 2 stays in extra
    std::cout << "after merge keys:";
    for (auto& [k, v] : names) std::cout << " " << k << "=" << v;  // read-only walk
    std::cout << "\nleftover in extra (duplicate 2):";
    for (auto& [k, v] : extra) std::cout << " " << k << "=" << v;
    std::cout << "\n";
}
