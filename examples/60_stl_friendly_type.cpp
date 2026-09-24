// =============================================================================
// WHAT  : A T that can live in vector (sort), set (operator<), unordered_set
//         (hash + ==). noexcept move so vector realloc does not copy.
// WHY   : set uniqueness is operator<. unordered_set uniqueness is hash then
//         ==. If hash(a)!=hash(b) when a==b, contains misses. If < and ==
//         disagree, you will lose your mind. Name is not part of identity here.
// THREAD: Local containers. If Item is in a shared set, insert is a write.
// ERASE : Not shown. set: it = s.erase(it). unordered_set: same; do not insert
//         during a walk. vector: erase-remove by id, or find then erase.
// REMEMBER: hash only id because == only id. Move noexcept. set insert of the
//           same id is a no-op (second name dropped).
// Guide : 08_types_in_containers.md, 05_hash_compare_lookup.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

struct Item {
    int id;
    std::string name;

    Item(int i, std::string n) : id(i), name(std::move(n)) {}

    Item(const Item&) = default;
    Item(Item&&) noexcept = default;  // vector realloc stays cheap
    Item& operator=(const Item&) = default;
    Item& operator=(Item&&) noexcept = default;

    friend bool operator<(const Item& a, const Item& b) { return a.id < b.id; }
    friend bool operator==(const Item& a, const Item& b) { return a.id == b.id; }
};

template <>
struct std::hash<Item> {
    std::size_t operator()(const Item& x) const noexcept {
        return std::hash<int>{}(x.id);  // same fields as ==
    }
};

int main() {
    std::vector<Item> v;
    v.reserve(3);
    v.emplace_back(2, "grace");
    v.emplace_back(1, "ada");
    std::sort(v.begin(), v.end());  // uses operator<
    std::cout << "sorted ids:";
    for (auto& x : v) std::cout << " " << x.id;
    std::cout << "\n";

    std::set<Item> ordered;
    ordered.insert(Item{3, "al"});
    ordered.insert(Item{3, "ignored duplicate id"});
    std::cout << "set size=" << ordered.size() << " (duplicate id dropped)\n";

    std::unordered_set<Item> hashed;
    hashed.insert(Item{9, "x"});
    std::cout << "hash contains id 9=" << std::boolalpha
              << hashed.contains(Item{9, "other name same id"}) << "\n";
}
