# 00. Mental model: a container is a cost contract

The STL is not a pile of classes. It is three layers that fit
together like a socket and a plug:

```
  +---------------------------+
  |  ALGORITHM                |   "what to do"   sort, find, merge
  |  speaks only iterators    |
  +-------------+-------------+
                |
                |  iterator = a position + a movement rule
                v
  +---------------------------+
  |  CONTAINER                |   "where it lives"  vector, map, ...
  |  owns memory, gives       |
  |  begin() / end()          |
  +---------------------------+
```

If you remember one sentence: **pick the container for the
operations that run in the hot loop; pick the algorithm so you do
not rewrite that loop.**

Never used the STL? Start at [`00_beginner.md`](00_beginner.md).
How to turn a problem statement into a container:
[`14_read_a_problem.md`](14_read_a_problem.md).
Three reading tracks: [`tracks.md`](tracks.md).

## What you already need

- You can compile `c++ -std=c++20 file.cpp`.
- You know `for`, functions, `struct`, and that `vector` is "a
  resizable array".
- You do not need templates, allocators, or move semantics yet.
  They show up in [`08`](08_types_in_containers.md) and
  [`09`](09_memory_reserve_pmr.md) at the point of use.

## Cost is a shape, not a number

Big-O without a picture is trivia. These three layouts make the
same `find(7)` look like three different machines:

```
  vector (scan a row)              list (chase pointers)
  [ 3 | 1 | 7 | 4 ]                [3] -> [1] -> [7] -> [4]
    sequential bytes                 each hop = a cache miss

  map / set (walk a tree)
            [4]
           /   \
         [1]   [7]
         /
       [3]
    log n comparisons, pointer-chasing
```

`vector` find is O(n) but often **wins in wall-clock** against a
tree find of O(log n) until n is large, because one cache line
holds many ints. That is the first critical-thinking rule in this
guide:

> Asymptotics pick the *family*. Constants and cache pick the
> *winner inside a family*. Default to `vector` until a measured
> operation is the wrong shape (front insert, middle splice,
> log-n predecessor, average O(1) key lookup).

## CRUD is the same four questions on every type

| Letter | Question | Typical verbs |
| --- | --- | --- |
| C | How does a value enter? | `push_back`, `insert`, `emplace`, `operator[]` |
| R | How do I get it back? | `[]`, `find`, `top`, `front`, `lower_bound` |
| U | How do I change it in place? | `v[i]=`, `it->second=` |
| D | How does a value leave? | `pop_back`, `erase`, `clear` |

If a type **cannot** do one of these cheaply, that absence is the
design. `priority_queue` has no "delete the value 7" because a
binary heap cannot find 7 in O(log n). `set` has no `operator[]`
because there is no positional index.

## Ownership vs view

```
  OWNER                         VIEW
  vector / string / map         span / string_view
  dies => memory dies           dies => memory still there
  can grow                      cannot grow
```

Pass `span<const T>` into helpers. Store **owners** in long-lived
objects. Mixing this up is use-after-free, not a style issue.

## A tiny program that uses all three layers

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{3, 1, 4, 1, 5};   // container owns
    auto it = std::find(v.begin(), v.end(), 4);  // algorithm + iterators
    if (it != v.end())
        std::cout << "found at index " << (it - v.begin()) << "\n";
}
```

`it - v.begin()` is legal **only** because `vector::iterator` is
random-access. On a `list` that subtraction does not compile. The
compiler is enforcing the cost contract. Chapter
[`01_iterators.md`](01_iterators.md) is that contract in full.

## How a chapter is built

Each container / algorithm file repeats this skeleton:

1. Picture of memory
2. CRUD table (or "what the algorithm reads/writes")
3. One job it is the best tool for
4. **Why not the obvious alternative**
5. Related problems (same shape, different story)
6. A path to a file in `examples/` you can run

If you only have thirty minutes, read the picture and the "why
not" box. The methods you can look up. The *choice* you have to
practice.

After the first container chapter, the mechanics that sit *behind*
CRUD are [`03_iterator_adapters.md`](03_iterator_adapters.md)
through [`13_parallel_and_cpp23.md`](13_parallel_and_cpp23.md).
Finish with [`capstone.md`](capstone.md) and
[`exercises.md`](exercises.md). The dictionary is
[`glossary.md`](glossary.md). The laws are
[`rules.md`](rules.md). The one-page paper is
[`cheat_sheet.md`](cheat_sheet.md).
