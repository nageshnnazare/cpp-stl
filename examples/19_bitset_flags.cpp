// =============================================================================
// WHAT  : bitset flags, Hamming distance as popcount of XOR, tiny sieve.
// WHY   : N is compile-time. Packed bits, word-wise &, |, ^, count().
//         vector<bool> is a trap. Runtime N → vector<char>. N<=64 → unsigned.
// THREAD: Concurrent writes to different bits of the SAME bitset are a data
//         race (they may share a word). Index loop in one thread is fine.
// ERASE : reset(i) / reset() — no iterators in C++20. Index loops are the walk.
// REMEMBER: bit 0 is LSB. test(i) throws if i>=N; [] need not. to_ulong
//           throws overflow_error if N is too wide.
// Guide : containers/bitset.md, 02_decision_guide.md
// =============================================================================
#include <bitset>
#include <iostream>

int main() {
    enum : std::size_t { Read = 0, Write = 1, Exec = 2 };
    std::bitset<3> perm;
    perm.set(Read).set(Exec);
    std::cout << "perm=" << perm << " write=" << perm.test(Write)
              << " count=" << perm.count() << "\n";

    std::bitset<8> a{0b10110010};
    std::bitset<8> b{0b10010011};
    std::cout << "hamming(a,b)=" << (a ^ b).count() << "\n";

    constexpr int N = 30;
    std::bitset<N + 1> composite;
    for (int p = 2; p * p <= N; ++p)
        if (!composite[p])
            for (int m = p * p; m <= N; m += p) composite.set(m);
    std::cout << "primes <= " << N << ":";
    for (int i = 2; i <= N; ++i)
        if (!composite[i]) std::cout << " " << i;
    std::cout << "\n";
}
