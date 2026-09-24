# Sorting, selecting, partitioning

Header: `<algorithm>`. These need **random-access** iterators
(`sort`, `nth_element`, `partial_sort`) except `is_sorted`,
which is a scan.

```
  unsorted     [ 7 | 1 | 5 | 3 | 9 | 2 ]

  sort         [ 1 | 2 | 3 | 5 | 7 | 9 ]

  nth_element n=3 (0-based: the 4th)
               [ 1 | 2 | 3 | 5 | 9 | 7 ]
                             ^ a[3]==5, left ≤5, right ≥5, sides unsorted

  partial_sort k=3
               [ 1 | 2 | 3 | ? | ? | ? ]
                 sorted prefix     rest unspecified
```

## Complexity and extra memory

| Algorithm | Time | Extra | Stable | Needs |
| --- | --- | --- | --- | --- |
| `sort` | O(n log n) | O(log n) | no | random access |
| `stable_sort` | O(n log n) | O(n) typical | **yes** | random access |
| `partial_sort` | O(n log k) | O(1) | no | random access |
| `partial_sort_copy` | O(n log min(n,k)) | writes k | no | input + random dest |
| `nth_element` | avg O(n) | O(1) | no | random access |
| `is_sorted` / `is_sorted_until` | O(n) | O(1) | — | forward |

libstdc++ `sort` is introsort (quick + heap + insertion). Worst
case is O(n log n), not the textbook O(n²) quicksort.

`list::sort` / `forward_list::sort` are merge sorts on nodes.
Use the **member**.

## Job 1 — pick the algorithm by the question

```
  "print everyone ordered" -------------------- sort
  "print everyone, keep original order of ties"  stable_sort
  "who is 0-based k-th?" ---------------------- nth_element
  "smallest k, and I will print them sorted" -- partial_sort
  "smallest k, order among them does not matter" nth_element
                                               then the prefix is the k
  "k from a stream I cannot store" ------------ heap of size k
```

```cpp
std::nth_element(v.begin(), v.begin() + k, v.end());
int kth = v[k];
```

Median: `nth_element` at `n/2`. Two heaps if the data is a
stream — [`recipes/median_stream.md`](../recipes/median_stream.md).

## Job 2 — `stable_sort` when the key is not the whole object

Sort students by grade, keep enrollment order among same grade.

```
  (Ann, B) (Bob, A) (Cyd, B) (Dan, A)
  stable_sort by grade desc:
  (Bob, A) (Dan, A) (Ann, B) (Cyd, B)
           Bob before Dan because he was before
```

Unstable `sort` may swap Bob and Dan. If you cannot pay O(n)
extra, decorate with the original index and `sort` on
`(grade, index)` — that is a Schwartzian transform and is
stable by construction.

## Job 3 — partition (see also [`partition.md`](partition.md))

`sort` is the heavy hammer for "trues then falses". `partition`
is linear:

```cpp
auto mid = std::partition(v.begin(), v.end(),
                          [](int x){ return x % 2 == 0; });
// [ evens | odds ]   *mid is first odd
```

Dutch national flag (0/1/2) is three-way partition, not `sort`.

## Comparator rules

```cpp
std::sort(v.begin(), v.end(), [](const auto& a, const auto& b){
    return a.score > b.score;   // strict weak ordering
});
```

Must be a **strict** `<`: never `<=` (that breaks uniqueness of
equivalents and is undefined). Equality is `!cmp(a,b) &&
!cmp(b,a)`.

## Why this, not that

| Need | Choose | Do not |
| --- | --- | --- |
| full order, scalars | `sort` | `set` insert one-by-one unless you needed the set later |
| k-th | `nth_element` | `sort` the whole thing |
| top-k sorted | `partial_sort` or heap | `sort` if n is huge |
| keep tie order | `stable_sort` or index decorate | hope `sort` is stable (it is not) |
| already almost sorted | `sort` still fine (insertion tail) | write your own insertion sort |

## Related problems

- Kth largest (nth_element or heap)
- Sort colors (partition, not sort)
- Merge intervals (sort by start)
- Largest number (sort with custom `a+b vs b+a`)
- Queue reconstruction by height (`stable_sort` + inserts)
- Pancake / wiggle sort (`nth_element` + rearrange)

Run [`examples/22_algo_sort_select.cpp`](../examples/22_algo_sort_select.cpp).
