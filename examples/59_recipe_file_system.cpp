// =============================================================================
// WHAT  : In-memory FS: nested map<name, Node> trie. ls is ordered because map.
// WHY   : Paths are hierarchical. unordered_map would scramble ls. A flat
//         map<path, content> cannot mkdir intermediate dirs cheaply or ls a
//         directory without scanning every path. unique_ptr children own the tree.
// THREAD: mkdir/write/rm write. ls/read can be const if you freeze the tree.
//         Two writers on the same node = race. Lock the FS.
// ERASE : rm finds the parent, then ch.erase(it) — iterator erase, not
//         range-for+erase. Non-empty dirs refused so we do not walk-and-delete
//         children in this demo. To rm -r: collect names, then erase (pattern 3)
//         or recurse then erase the child.
// REMEMBER: map keys are the ls order. walk(..., create, last_file) builds
//           missing dirs. find, not [], when a miss must not create a node.
// Guide : recipes/file_system.md, containers/map.md
// =============================================================================
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class FileSystem {
    struct Node {
        std::map<std::string, std::unique_ptr<Node>> ch;
        std::string data;
        bool is_file = false;
    };
    Node root_;

    static std::vector<std::string> split(std::string_view p) {
        std::vector<std::string> parts;
        std::string cur;
        for (char c : p) {
            if (c == '/') {
                if (!cur.empty()) parts.push_back(cur);
                cur.clear();
            } else {
                cur.push_back(c);
            }
        }
        if (!cur.empty()) parts.push_back(cur);
        return parts;
    }

    Node* walk(const std::vector<std::string>& parts, bool create_dirs, bool last_file) {
        Node* n = &root_;
        for (std::size_t i = 0; i < parts.size(); ++i) {
            bool last = i + 1 == parts.size();
            auto it = n->ch.find(parts[i]);
            if (it == n->ch.end()) {
                if (!create_dirs) return nullptr;
                auto child = std::make_unique<Node>();
                if (last && last_file) child->is_file = true;
                it = n->ch.emplace(parts[i], std::move(child)).first;
            }
            n = it->second.get();
            if (!last && n->is_file) return nullptr;
        }
        return n;
    }

    Node* parent_and_name(const std::vector<std::string>& parts, std::string& name) {
        if (parts.empty()) return nullptr;
        name = parts.back();
        Node* n = &root_;
        for (std::size_t i = 0; i + 1 < parts.size(); ++i) {
            auto it = n->ch.find(parts[i]);
            if (it == n->ch.end() || it->second->is_file) return nullptr;
            n = it->second.get();
        }
        return n;
    }

public:
    bool mkdir(const std::string& path) {
        auto parts = split(path);
        return walk(parts, true, false) != nullptr;
    }

    std::vector<std::string> ls(const std::string& path) {
        auto parts = split(path);
        Node* n = parts.empty() ? &root_ : walk(parts, false, false);
        if (!n) return {};
        if (n->is_file) return {parts.back()};
        std::vector<std::string> out;
        for (auto& [k, _] : n->ch) out.push_back(k);  // map order = alpha ls
        return out;
    }

    bool write(const std::string& path, const std::string& content) {
        auto parts = split(path);
        if (parts.empty()) return false;
        Node* n = walk(parts, true, true);
        if (!n) return false;
        n->is_file = true;
        n->data += content;
        return true;
    }

    std::string read(const std::string& path) {
        auto parts = split(path);
        Node* n = walk(parts, false, false);
        if (!n || !n->is_file) return "";
        return n->data;
    }

    bool rm(const std::string& path) {
        auto parts = split(path);
        std::string name;
        Node* p = parent_and_name(parts, name);
        if (!p) return false;
        auto it = p->ch.find(name);
        if (it == p->ch.end()) return false;
        if (!it->second->is_file && !it->second->ch.empty()) return false;
        p->ch.erase(it);  // after we decided; it is not incremented
        return true;
    }
};

int main() {
    FileSystem fs;
    fs.mkdir("/a/b");
    fs.write("/a/b/c.txt", "hello");
    fs.write("/a/b/c.txt", "!");
    fs.write("/a/d.txt", "x");
    std::cout << "ls /a:";
    for (auto& n : fs.ls("/a")) std::cout << " " << n;
    std::cout << "\nread c.txt=" << fs.read("/a/b/c.txt") << "\n";
    std::cout << "rm nonempty /a/b=" << std::boolalpha << fs.rm("/a/b") << "\n";
    std::cout << "rm c.txt=" << fs.rm("/a/b/c.txt") << " ls /a/b:";
    for (auto& n : fs.ls("/a/b")) std::cout << " " << n;
    std::cout << "\n";
}
