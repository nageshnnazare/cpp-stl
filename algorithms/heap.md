# Heap algorithms — `priority_queue` with the lid off

Header: `<algorithm>`. The range must be **random-access**.
A heap is a binary tree stored in an array:

```
  index:   0  1  2  3  4  5
  value:   9  7  8  1  3  6

            9                 parent(i) = (i-1)/2
          /   \               left(i)   = 2i+1
         7     8              right(i)  = 2i+2
        / \   /
       1   3 6

  max-heap:  parent ≥ children
```

| Algorithm | Does | Time |
| --- | --- | --- |
| `make_heap` | in-place build | O(n) |
| `push_heap` | after `push_back` of the new element | O(log n) |
| `pop_heap` | swap max to `end-1`, reheap `[first, end-1)` | O(log n) |
| `sort_heap` | repeated `pop_heap` → sorted | O(n log n) |
| `is_heap` / `is_heap_until` | check | O(n) |

Default comparator `less<T>` ⇒ **max-heap** (same as
`priority_queue`).

## Job — when you need the vector *and* the heap

`priority_queue` hides the vector. Use raw heap ops when you
must:

- inspect every element (`pq` has no iterators)
- `make_heap` once on existing data (O(n) vs n × `push`)
- `sort_heap` as the last step of heapsort
- implement "heap + lazy deleted flags" sitting in the same
  buffer

```cpp
std::vector<int> h{3, 1, 4, 1, 5};
std::make_heap(h.begin(), h.end());     // 5 ...
h.push_back(9);
std::push_heap(h.begin(), h.end());     // 9 on top
std::pop_heap(h.begin(), h.end());
int mx = h.back(); h.pop_back();
```

```
  push_heap:  you already appended
  [ 5 4 3 1 1 | 9 ]
                ^
  sift 9 up -> [ 9 4 5 1 1 3 ]

  pop_heap:
  swap 9 with last, sift down the hole
  [ 5 4 3 1 1 | 9 ]      9 is sitting in the "sorted" tail
```

## Why `make_heap` not n times `push_heap`

Floyd's build is O(n). n insertions are O(n log n). If the data
is already in a `vector`, **build**. If it arrives as a stream,
**push**.

## Why this, not `sort`

Heapsort (`make_heap` + `sort_heap`) is O(n log n), not stable,
worse constants than `std::sort`. You use heap **algorithms**
for the online "give me the current max" shape, not to sort a
static array.

## Related problems

- Last stone weight (pop two, push difference)
- K-th largest in a stream (`KthLargest` class)
- Heap sort as a teaching exercise
- "Ugly numbers" (multi-pointer or min-heap of candidates)

Run [`examples/24_algo_heap.cpp`](../examples/24_algo_heap.cpp).
