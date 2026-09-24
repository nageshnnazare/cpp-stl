// =============================================================================
// WHAT  : reserve vs growth; clear keeps capacity; swap-with-empty releases;
//         pmr::vector on a stack monotonic arena.
// WHY   : Unreserved push_back reallocates (and invalidates every iterator /
//         pointer / span into the vector). reserve(n) = one allocation if you
//         know n. PMR: the container rules stay the same; only the allocator
//         changes. PMR does not add locks.
// THREAD: Same as vector: CR, no W, elem-if-no-realloc. The arena is not a
//         mutex. Two pmr containers sharing one resource need external locking
//         of both the containers AND the resource if they allocate.
// ERASE : clear() does not shrink. erase still invalidates from the hole
//         onward. PMR vector erase is the same as std::vector erase.
// REMEMBER: After reserve, push_back does not invalidate until size hits cap.
//           monotonic_buffer_resource never frees individual ints — the arena
//           dies as a whole. Do not let pmr_v outlive `buf`.
// Guide : 09_memory_reserve_pmr.md, 02_decision_guide.md (pmr row)
// =============================================================================
#include <iostream>
#include <memory_resource>
#include <vector>

int main() {
    std::vector<int> a;
    for (int i = 0; i < 100; ++i) a.push_back(i);  // several reallocs; iterators would die
    std::cout << "no reserve: size=" << a.size() << " cap=" << a.capacity() << "\n";

    std::vector<int> b;
    b.reserve(100);
    for (int i = 0; i < 100; ++i) b.push_back(i);  // no realloc; pointers stay valid
    std::cout << "reserve(100): size=" << b.size() << " cap=" << b.capacity() << "\n";

    b.clear();
    std::cout << "after clear cap still " << b.capacity() << "\n";
    std::vector<int>().swap(b);  // the trick to actually free
    std::cout << "after swap-with-empty cap=" << b.capacity() << "\n";

    char buf[1024];
    std::pmr::monotonic_buffer_resource arena(buf, sizeof buf);
    std::pmr::vector<int> pmr_v{&arena};
    pmr_v.reserve(32);
    for (int i = 0; i < 32; ++i) pmr_v.push_back(i);
    std::cout << "pmr vector size=" << pmr_v.size()
              << " (storage in stack buf until overflow)\n";
}
