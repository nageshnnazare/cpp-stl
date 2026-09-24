# 00b. First hour: you have never used the STL

This page is for a reader who can write a `for` loop and a
`struct`, and who has heard "vector is a resizable array",
and nothing else. After one sitting you can compile a
program, grow a `vector`, look a key up in a map, and know
where the rest of this bible lives.

The cost-contract picture is
[`00_mental_model.md`](00_mental_model.md). Come back here
if that page felt early.

## What "the STL" is

The **Standard Template Library** is the part of C++ in
namespace `std` that gives you:

```
  containers     where values live     vector, map, unordered_map, ...
  iterators      a position in there   v.begin(), v.end()
  algorithms     loops with names      sort, find, count
```

You do **not** write `new[]` / `delete[]` for a list of ints.
You do **not** write a linked-list class for BFS. You pick a
container whose **hot operation** is cheap, then call an
algorithm instead of a hand-rolled loop when the name fits.

Everything lives in a **header**. You include only what you
use:

```
  <vector>           <deque>        <list>           <array>
  <string>           <string_view>  <span>           <bitset>
  <set>              <map>          <unordered_set>  <unordered_map>
  <stack>            <queue>        <span>
  <algorithm>        <numeric>      <iterator>       <ranges>
  <utility>          <tuple>        <optional>
```

`<queue>` is also where `priority_queue` lives. There is no
`<priority_queue>`.

Compile with C++20:

```bash
c++ -std=c++20 -Wall -Wextra -Wpedantic file.cpp -o file
./file
```

From this directory: `make` then `./build/80_beginner_hello`.

## Program 1 — a growable list of numbers

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v;          // empty, owns its buffer
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    std::cout << "size=" << v.size() << " first=" << v[0]
              << " last=" << v.back() << "\n";

    for (int x : v)              // range-for: every element, in order
        std::cout << x << " ";
    std::cout << "\n";
}
```

Memorize:

| You write | Meaning |
| --- | --- |
| `v.push_back(x)` | append; may grow the buffer |
| `v.size()` | how many elements |
| `v[i]` | the i-th, **no** bounds check |
| `v.at(i)` | the i-th, throws if `i >= size` |
| `v.front()` / `v.back()` | first / last — UB if empty |
| `v.pop_back()` | drop last — UB if empty |
| `for (int x : v)` | copy each; `for (int& x : v)` to write |

`v[i]` on an empty vector is undefined behavior, not "0".
`push_back` first, or `v.resize(n)` to create n default
elements.

## Program 2 — look a word up

```cpp
#include <iostream>
#include <string>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, int> freq;
    ++freq["cat"];
    ++freq["cat"];
    ++freq["dog"];

    std::cout << "cat=" << freq["cat"] << "\n";

    if (freq.contains("bird"))   // C++20; no insert
        std::cout << "have bird\n";
    else
        std::cout << "no bird\n";
}
```

`++freq["cat"]` is the **one** place `operator[]` is the
right tool: missing key becomes `0`, then you add one.

To **test** membership without inserting:

```cpp
if (auto it = freq.find("bird"); it != freq.end())
    std::cout << it->second << "\n";   // the count
```

`find` returns an **iterator**. "Not found" is `end()`, not
a boolean. `contains` is the boolean.

**`map` vs `unordered_map` for a beginner:**

```
  unordered_map    average O(1) find     keys in hash order (useless)
  map              O(log n) find         keys walk smallest → largest
```

Need "print keys alphabetically"? `map`. Need "count words
as fast as possible"? `unordered_map`. When unsure and n is
small, either works; this bible defaults to `unordered_map`
for lookup-only.

## Range-for, iterators, and `end()`

```
  v:     [ 10 | 20 | 30 ]
           ^              ^
         begin()        end()     end is ONE PAST the last
```

```cpp
for (auto it = v.begin(); it != v.end(); ++it)
    std::cout << *it << "\n";    // *it is the element
```

Range-for is that loop with the iterator hidden. You almost
never write the iterator loop on a `vector` unless you are
erasing (see the decision guide). You **never** write
`*v.end()`.

## The only three containers you need this week

| Container | Picture | Use it for |
| --- | --- | --- |
| `vector<T>` | row of T in memory | almost everything: lists, stacks (`back()`), buffers |
| `unordered_map<K,V>` | hash table | "id → value", frequency, "have I seen this key" |
| `unordered_set<K>` | hash set | "have I seen this value" |

Next three, when the first three are the wrong *shape*:

| Container | When the first three fail |
| --- | --- |
| `deque<T>` | push/pop **both** ends (queue, sliding window) |
| `priority_queue<T>` | always want the **current max** (or min), stream of values |
| `set<K>` / `map<K,V>` | need **sorted** keys, closest key, min **and** delete arbitrary |

`list` is not in the first six. You need it when you already
hold a handle to a node and must cut it out in O(1) without
moving everyone else (LRU). That sentence is rare.

## `sort` is one line

```cpp
#include <algorithm>
#include <vector>

std::vector<int> v{3, 1, 4, 1};
std::sort(v.begin(), v.end());          // 1 1 3 4
std::sort(v.begin(), v.end(), std::greater<int>{});  // 4 3 1 1
```

`sort` needs a `vector` / `deque` / `array` (random-access).
It will **not compile** on `list` or `set`. `set` is already
sorted; `list` has `lst.sort()`.

## Compile errors you will hit today

| The compiler said (paraphrased) | You did |
| --- | --- |
| `sort` no matching function, list iterators | `std::sort` on a `list` — use `lst.sort()` or copy to a vector |
| `operator[]` cannot be used on `const` map | `[]` may insert; use `find` / `at` / `contains` |
| no `hash` for `pair<int,int>` | write a hasher, or encode the pair as a `long long` |
| `back_inserter` / `copy` into empty `begin()` | `copy` **writes**, it does not grow; use `back_inserter` |
| `unique_ptr` in `vector{ p1, p2 }` | `initializer_list` copies; emplace in a loop |

A red error inside `stl_vector.h` is almost always **your**
`T` missing a copy/move/`<`/`hash`. Read
[`08_types_in_containers.md`](08_types_in_containers.md)
when that happens — not the STL header.

## Mental rules that save beginners

1. **Default to `vector`.** Prove you need something else.
2. **`[]` on a map is a write.** Membership is `find` /
   `contains`.
3. **Do not `erase` inside `for (x : c)`.** That is undefined.
   See [`02_decision_guide.md`](02_decision_guide.md).
4. **Empty `.front()` / `.back()` / `.top()` / `.pop()` is UB.**
   Check `empty()` first.
5. **`string_view` / `span` do not own.** If the `string` dies
   or grows, the view is garbage.
6. **n = 1e6 of `int` in a `vector` is fine** (~4 MB). The same
   n in a `map` is tens of MB and slower. Do not "upgrade" to
   a tree because it feels advanced.

## Where to go next (beginner track)

```
  this page + example 80
       |
       v
  00_mental_model.md          picture of cost
  02_decision_guide.md        the picker (skim the tables)
  14_read_a_problem.md        ASK / ARRIVE / EXTRA (do not invent CRUD)
  containers/vector.md        your default
  containers/unordered_map.md frequency / Two Sum
  recipes/two_sum_and_anagrams.md
  examples/01 and 10          run them
```

Weekend / interview / expert tracks:
[`tracks.md`](tracks.md). The numbered laws:
[`rules.md`](rules.md). One-page paper:
[`cheat_sheet.md`](cheat_sheet.md).

Practice: [`examples/80_beginner_hello.cpp`](examples/80_beginner_hello.cpp).
