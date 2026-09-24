// =============================================================================
// WHAT  : partition, stable_partition, partition_point.
// WHY   : "True values in front" without a full sort. Quickselect's cousin.
//         Dutch national flag / odd-even split / Hoare partition live here.
// THREAD: In-place swaps. Exclusive ownership of the vector.
// ERASE : Partition does not erase. After partition, you may erase the false
//         side: v.erase(mid, v.end()) — that is another way to filter.
//         Do not erase inside the predicate (it would see a moving range).
// REMEMBER: partition is unstable. stable_partition keeps relative order.
//           partition_point is binary search *assuming already partitioned*.
// Guide : algorithms/partition.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <vector>

void show(const char* label, const std::vector<int>& v) {
    std::cout << label;
    for (int x : v) std::cout << " " << x;
    std::cout << "\n";
}

int main() {
    std::vector<int> v{7, 2, 5, 8, 4, 1};
    auto mid = std::partition(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    show("partition evens first", v);
    std::cout << "first odd=" << *mid << " at index " << (mid - v.begin()) << "\n";

    std::vector<int> s{7, 2, 5, 8, 4, 1};
    std::stable_partition(s.begin(), s.end(), [](int x) { return x % 2 == 0; });
    show("stable_partition (evens keep order)", s);

    std::vector<int> flags{0, 0, 0, 1, 1, 1};  // already partitioned: <1 then >=1
    auto p = std::partition_point(flags.begin(), flags.end(), [](int x) { return x < 1; });
    std::cout << "partition_point first 1 at " << (p - flags.begin()) << "\n";
}
