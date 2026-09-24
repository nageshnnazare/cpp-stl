# Freq stack — map of counts + **stack per frequency**

**Need:** `push(x)` / `pop()` where `pop` returns the **most
frequent** remaining value; ties go to the **most recently
pushed** of those.

A min/max heap of counts fails the tie ("most recent") unless
you also store timestamps and delete-from-heap. The STL-shaped
answer is:

```
  freq[x] = how many of x are currently in the structure
  bucket[f] = stack of values whose *current* freq is f
  maxf = highest f that still has a non-empty stack
```

```
  push 5, 7, 5, 7, 4, 5

  freq:  5:3  7:2  4:1
  bucket[1]:  5  7  4
  bucket[2]:  5  7
  bucket[3]:  5
  maxf=3

  pop -> 5 (only one at freq 3), maxf becomes 2
  pop -> 7 (top of freq 2, more recent than the 5 sitting under it)
```

Each value lives on **every** stack up to its count (5 sits on
1, 2, and 3). Pop peels one level.

## Why not a heap, not one stack

| Design | pop most frequent | recency tie | Verdict |
| --- | --- | --- | --- |
| one `stack` | no | yes | wrong extreme |
| `priority_queue` (freq, time, val) | yes | yes | works O(log n); must update freq |
| **`unordered_map` freq + `vector<stack>`** | O(1) | O(1) | **the one** |
| `map<freq, vector>` | O(log F) | O(1) | fine if you dislike holes in a vector |

`bucket` as `vector<stack<int>>` indexed by frequency: grow
with `resize(maxf+1)`. Frequency of any one value is ≤ n.

## Sketch

```cpp
std::unordered_map<int, int> freq;
std::vector<std::stack<int>> bucket;  // bucket[f]
int maxf = 0;

void push(int x) {
    int f = ++freq[x];
    maxf = std::max(maxf, f);
    if ((int)bucket.size() <= f) bucket.resize(f + 1);
    bucket[f].push(x);
}

int pop() {
    int x = bucket[maxf].top();
    bucket[maxf].pop();
    --freq[x];
    if (bucket[maxf].empty()) --maxf;
    return x;
}
```

Full program: [`examples/49_recipe_freq_stack.cpp`](../examples/49_recipe_freq_stack.cpp).

## Related problems

- Maximum Frequency Stack (895)
- Min Stack (155) — [`min_stack.md`](min_stack.md); extreme is
  value, not frequency
- LFU Cache — frequency *plus capacity eviction*; lists not
  stacks ([`lfu_cache.md`](lfu_cache.md))
- Reorganize string — heap of global remaining counts, no
  recency ([`task_scheduler.md`](task_scheduler.md))
- Freq stack vs leaderboard: here pop **mutates** counts; a
  leaderboard `top(K)` is a read
