# Sliding window — a subrange that only grows/shrinks at the ends

The window is `[left, right]` on a `string` or `vector`.
`right` advances in a `for`. `left` catches up while a
constraint is broken. Extra container = whatever the constraint
needs to query in O(1) / amortized O(1).

```
  right ------------------>
  [ a | b | c | a | b | c | b | b ]
        ^ left moves when duplicate 'a' enters

  need:  last index of each char, or a count map
```

## Constraint → container

| Window question | Extra structure | Why |
| --- | --- | --- |
| max / min of values | **monotonic `deque` of indices** | pops from both ends |
| max / min, simpler | `multiset` of window values | O(n log k), easy erase |
| sum / product / xor | a scalar | maybe a `vector` prefix |
| all unique chars | `unordered_map`/`array` counts + `unique` counter | |
| covers a pattern (need counts) | `array<int,26>` or hash of need/window | "minimum window substring" |
| anagrams of p in s | same counts, `matches` integer | "find all anagrams" |
| at most k distinct | hash of counts | shrink when `map.size()>k` |

## Why a deque for window maximum

```
  nums  1  3  -1  -3  5  3  6  7     k=3
  deque stores indices, values **decreasing**
  front = max of current window

  i=0  [0:1]
  i=1  3>1 pop 0,  [1:3]
  i=2  [1:3, 2:-1]
  i=3  drop i=0 if still... 1 is out; [1,2,3] -> 3 is out later...
```

A heap of `(value, index)` needs lazy skip of indices `< left`.
Correct, O(n log k). Deque is amortized O(1) per index: each
index is pushed once and popped once.

**Why not `queue`?** You must delete from the back when the new
value dominates. **Why not `vector`?** Popping front is O(k).

## Minimum window substring (counts)

```
  s = "ADOBECODEBANC"   t = "ABC"
  need {A:1,B:1,C:1}   missing=3

  expand right, --missing when a needed char is satisfied
  while missing==0:  record, shrink left
```

`unordered_map<char,int>` or two `array<int,128>`. ASCII array
is faster; Unicode → map.

## Related problems

- Longest Substring Without Repeating (3)
- Minimum Window Substring (76), Find All Anagrams (438)
- Sliding Window Maximum (239)
- Longest Repeating Character Replacement (424)
- Max consecutive ones with k flips (1004)
- Fruit into baskets (904) — at most 2 distinct
- Moving average (easy queue)

Run [`examples/33_recipe_sliding_window.cpp`](../examples/33_recipe_sliding_window.cpp).
