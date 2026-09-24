// =============================================================================
// WHAT  : array<int,26> as an anagram signature; fixed lookup table.
// WHY   : N is a compile-time constant. No heap. operator== compares all bins.
//         unordered_map<char,int> here is a hash + node for a 26-wide domain.
// THREAD: Concurrent writes to different a[i] are OK (no realloc ever).
//         The object is still not internally locked as a whole for mix of
//         readers/writers of the same slot.
// ERASE : Cannot shrink. "Delete" = set a slot to 0 / fill a sentinel.
//         Index loops are fine; there is no erase() to invalidate iterators.
// REMEMBER: array<T,N> copies in O(N). Pass span<const T> into helpers.
//           Not vector: N is known. Not C array: .size() survives a function call.
// Guide : containers/array.md, 02_decision_guide.md
// =============================================================================
#include <array>
#include <iostream>
#include <string>
#include <string_view>

std::array<int, 26> counts(std::string_view s) {
    std::array<int, 26> c{};  // value-init every bin to 0
    for (char ch : s)
        if (ch >= 'a' && ch <= 'z') ++c[static_cast<std::size_t>(ch - 'a')];
    return c;
}

int main() {
    auto a = counts("anagram");
    auto b = counts("nagaram");
    auto c = counts("nagarmm");
    std::cout << "anagram vs nagaram: " << (a == b ? "same" : "diff") << "\n";
    std::cout << "anagram vs nagarmm: " << (a == c ? "same" : "diff") << "\n";

    std::array<const char*, 4> dir{"N", "E", "S", "W"};
    std::cout << "dir[2]=" << dir[2] << " size=" << dir.size() << "\n";
}
