# Capstone: one program, four containers

A log processor is the shortest program that is still a *design*.
You will count, rank, and slide a window — the same three
shapes as the recipes, in one `main`.

The method: [`14_read_a_problem.md`](14_read_a_problem.md).
From the **sentence** (not a CRUD form):

```
  ASK     count words, print top-k, then a sliding uniqueness window
  ARRIVE  a finite log (offline), then one window scan
  EXTRA   window of last K  ← without this, drop the deque

  delete of a word from freq is n/a
  "by user" is in the input but not in ASK — ignore user for counting
```

(The seven-slot sheet is optional leftovers: noun=word,
delete of a counted word = n/a, window pop_front is EXTRA
from "last K".)

```
  lines of (user, word)
       |
       v
  unordered_map<string, int>     freq          C/U  ++count
  vector<pair<int,string>>       ranked        then partial_sort
  deque<string>                  last K words  C/D  ends
  unordered_map<string, int>     counts in window   R  "already here?"
```

**Why this mix, not one `map`.** Frequency does not need order
while counting. Ranking needs a vector to `partial_sort` by
count. The window needs O(1) push/pop at the ends **and** O(1)
"is this word already in the window?" — deque + count map
(a `set` is not enough if the same word can appear twice in
the window: erase-from-set would drop the other copy).

```
  window K=3
  deque:       [ vector | map | vector ]
  counts:      vector:2  map:1

  next "set":  not in window
               pop vector (count 2->1), push set
  deque:       [ map | vector | set ]
```

Read the code: [`examples/70_capstone_logs.cpp`](examples/70_capstone_logs.cpp).
Then change K, swap `partial_sort` for a size-k heap, and
notice which container you had to touch. That is the whole
tutorial in one diff.

Related chapters: [`recipes/top_k_frequent.md`](recipes/top_k_frequent.md),
[`recipes/sliding_window.md`](recipes/sliding_window.md),
[`algorithms/sorting.md`](algorithms/sorting.md).

## You are done with this bible when

```
  [ ] Given a new problem, you underline ASK / ARRIVE / EXTRA
      and leave the rest n/a
  [ ] You do not add a container for a verb the statement never said
  [ ] You count axes *that were written*; destroy unhooks those,
      and skips destroy when delete is n/a
  [ ] You default to vector and can say why you left it
  [ ] map[] is never your membership test
  [ ] You can name the legal erase-while-walk (it = erase(it) / erase_if)
  [ ] You refuse priority_queue the moment the spec says "delete 7" / "reset id"
  [ ] You refuse list unless you already hold a handle
  [ ] You use member lower_bound on set/map
  [ ] You can quote R9, R18, R33, R40, R60 from rules.md
  [ ] You know when to leave the STL (15_expert.md)
```

Tracks: [`tracks.md`](tracks.md). Drills:
[`exercises.md`](exercises.md) **13–25**. Pocket:
[`cheat_sheet.md`](cheat_sheet.md).
