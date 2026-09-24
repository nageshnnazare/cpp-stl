# Algorithms

Algorithms take **iterators**, not containers. Complexity tables
for the whole library live in the [root README](../README.md).
Each file below is "when to call this instead of a `for`".

| File | Cluster |
| --- | --- |
| [`iterator_requirements.md`](iterator_requirements.md) | which iterator each algorithm needs |
| [`non_modifying.md`](non_modifying.md) | find, count, all_of, search |
| [`modifying.md`](modifying.md) | copy, transform, erase-remove, unique, rotate |
| [`sorting.md`](sorting.md) | sort, stable_sort, partial_sort, nth_element |
| [`binary_search.md`](binary_search.md) | lower_bound, answer-space search |
| [`heap.md`](heap.md) | make/push/pop_heap |
| [`partition.md`](partition.md) | partition, partition_point |
| [`set_operations.md`](set_operations.md) | merge, union, intersection |
| [`numeric.md`](numeric.md) | accumulate, prefix sums, iota |
| [`permutations.md`](permutations.md) | next_permutation, minmax |

The iterator contract is [`01_iterators.md`](../01_iterators.md).
If `std::sort` will not compile on your type, you do not have
random access — use a member (`list::sort`) or copy to a
`vector`. Which algorithm given a loop: the picker in
[`02_decision_guide.md`](../02_decision_guide.md) and
[`14_read_a_problem.md`](../14_read_a_problem.md).

Parallel overloads and C++23 `flat_map`:
[`13_parallel_and_cpp23.md`](../13_parallel_and_cpp23.md).
Predicates: [`10_predicates.md`](../10_predicates.md).
