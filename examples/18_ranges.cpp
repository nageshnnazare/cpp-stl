// =============================================================================
// WHAT  : Filter+transform view (lazy), materialize with copy, sort by projection.
// WHY   : The view does not own ints — it stores iterators into v. Use it when
//         you would throw the temporary vector away. Store? copy into a vector.
// THREAD: Walking a view while another thread push_back on v may realloc →
//         dangling. Same rule as iterators into v.
// ERASE : You cannot erase through a filter view. Erase on v, then rebuild
//         the view. sort(people) writes in place — exclusive ownership.
// REMEMBER: filter_view is not random-access — ranges::sort(odds_sq) will not
//           compile. Projection: sort by &Person::age without a lambda body.
//           ranges::find(temporary_vector, x) returns dangling — won't compile.
// Guide : 06_ranges.md, 07_pitfalls.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

struct Person {
    std::string name;
    int age;
};

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    auto odds_sq = v | std::views::filter([](int x) { return x % 2; }) |
                   std::views::transform([](int x) { return x * x; });
    std::cout << "odd squares:";
    for (int x : odds_sq) std::cout << " " << x;  // computed on the fly
    std::cout << "\n";

    std::vector<int> got;
    std::ranges::copy(odds_sq, std::back_inserter(got));  // now we own a copy
    std::cout << "materialized size=" << got.size() << "\n";

    std::vector<Person> people{{"Ann", 30}, {"Bob", 21}, {"Cyd", 30}};
    std::ranges::sort(people, {}, &Person::age);  // {} = ranges::less
    std::cout << "by age:";
    for (auto& p : people) std::cout << " " << p.name;
    std::cout << "\n";

    std::cout << "ranges::any_of even=" << std::boolalpha
              << std::ranges::any_of(v, [](int x) { return x % 2 == 0; }) << "\n";
}
