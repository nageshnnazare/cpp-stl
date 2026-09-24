// =============================================================================
// WHAT  : greater<> for reverse sort, lambda predicates, not_fn, ranges projection.
// WHY   : Algorithms take a callable, not a hard-coded <. greater<> is the
//         named reverse. not_fn wraps a predicate. Projection sort by &Member
//         without writing a lambda that returns a.age < b.age.
// THREAD: sort writes the range. any_of is a const scan. Exclusive vs writers.
// ERASE : Predicates must not erase from the range they are scanning.
//         not_fn does not allocate. sort invalidates nothing but reorders —
//         stored iterators still point at the same elements (now moved).
// REMEMBER: greater<> on a set makes rbegin the smallest. Projection is the
//           third argument of ranges::sort (or the 2nd of the comparator form).
// Guide : 10_predicates.md, 06_ranges.md
// =============================================================================
#include <algorithm>
#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

struct Person {
    std::string name;
    int age;
};

int main() {
    std::vector<int> v{3, 1, 4, 1, 5};
    std::sort(v.begin(), v.end(), std::greater<int>{});
    std::cout << "greater sort:";
    for (int x : v) std::cout << " " << x;
    std::cout << "\n";

    auto is_odd = [](int x) { return x % 2 != 0; };
    std::cout << "any_of odd=" << std::boolalpha
              << std::any_of(v.begin(), v.end(), is_odd) << "\n";
    std::cout << "any_of even via not_fn="
              << std::any_of(v.begin(), v.end(), std::not_fn(is_odd)) << "\n";

    std::vector<Person> people{{"Ann", 30}, {"Bob", 21}, {"Cyd", 25}};
    std::ranges::sort(people, std::less{}, &Person::age);
    std::cout << "by age:";
    for (auto& p : people) std::cout << " " << p.name;
    std::cout << "\n";
}
