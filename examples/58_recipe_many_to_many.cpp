// =============================================================================
// WHAT  : Many-to-many tags: item→tags and tag→items (two hashes of sets).
// WHY   : "tags of 7" and "items with stl" are two directions. One map cannot
//         answer both in O(1). Destroy an item MUST unhook every tag's set
//         or you leak ids. This is the dual-index rule in miniature.
// THREAD: add/remove/erase write both maps. items_with is a const snapshot.
//         Lock the pair. Do not [] in a const method (use find).
// ERASE : remove_tag erases from both sets; drops empty parent keys.
//         erase_item: walk the item's tag set (read-only), unhook each reverse
//         edge, THEN tags_of_.erase(it). Never erase from the set you are
//         iterating. Do not insert new tags during that walk (rehash).
// REMEMBER: Empty nested sets should die or find() lies about "live tags".
// Guide : recipes/many_to_many.md, recipes/three_plus.md
// =============================================================================
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class TagIndex {
    std::unordered_map<int, std::unordered_set<std::string>> tags_of_;
    std::unordered_map<std::string, std::unordered_set<int>> items_of_;

public:
    void add_tag(int item, const std::string& tag) {
        tags_of_[item].insert(tag);
        items_of_[tag].insert(item);
    }

    void remove_tag(int item, const std::string& tag) {
        if (auto it = tags_of_.find(item); it != tags_of_.end()) {
            it->second.erase(tag);
            if (it->second.empty()) tags_of_.erase(it);
        }
        if (auto it = items_of_.find(tag); it != items_of_.end()) {
            it->second.erase(item);
            if (it->second.empty()) items_of_.erase(it);
        }
    }

    void erase_item(int item) {
        auto it = tags_of_.find(item);
        if (it == tags_of_.end()) return;
        for (auto& tag : it->second) {  // read-only walk of this item's tags
            auto& xs = items_of_[tag];
            xs.erase(item);
            if (xs.empty()) items_of_.erase(tag);
        }
        tags_of_.erase(it);  // after the walk
    }

    std::vector<int> items_with(const std::string& tag) const {
        std::vector<int> out;
        if (auto it = items_of_.find(tag); it != items_of_.end())
            out.assign(it->second.begin(), it->second.end());
        return out;
    }

    int n_tags() const { return (int)items_of_.size(); }
};

int main() {
    TagIndex idx;
    idx.add_tag(7, "cpp");
    idx.add_tag(7, "stl");
    idx.add_tag(8, "stl");
    std::cout << "stl items:";
    for (int id : idx.items_with("stl")) std::cout << " " << id;
    std::cout << "  live tags=" << idx.n_tags() << "\n";
    idx.erase_item(7);
    std::cout << "after erase 7, stl items:";
    for (int id : idx.items_with("stl")) std::cout << " " << id;
    std::cout << "  live tags=" << idx.n_tags() << " (cpp gone)\n";
}
