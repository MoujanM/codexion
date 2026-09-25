*This project has been created as part of the 42 curriculum by mmirdama.*

# Codexion
Mastering the race for resources in a variation of the Dining Philosophers Problem

## Description

Codexion is a multithreaded C simulation inspired by the Dining Philosophers problem. A given number of coders compete for shared dongles to perform compile, debug, and refactor actions, powering up a central quantum compiler. A coder may only compile when holding two dongles, one in each hand. Dongles are released after each compile and are re-useable once a cooldown period has passed.

The goal is to build a multithreaded architecture that prevents deadlocks and avoids races, while ensuring all coders compile regularly and avoid burnout. The simulation uses configurable timing, scheduling, and burnout rules to model contention and resource starvation.

Key concepts covered in the project: concurrency, pthreads, mutex, race conditions

## Instructions

The program `codexion` is compiled with strict flags `-Wall -Wextra -Werror -pthread` on `make` command. 

Run the executable with the following arguments: 

```bash
make

./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]

# example
./codexion 5 800 200 200 100 3 100 fifo

# quick check for program end with no burnout
./codexion 5 800 200 200 100 3 100 fifo | tee out.log
grep "is compiling" out.log | awk '{print $2}' | sort | uniq -c

# valgrind checks
valgrind --tool=helgrind ./codexion 5 800 200 200 100 3 100 edf / fifo
valgrind --tool=drd ./codexion 5 800 200 200 100 3 100 edf / fifo

```
**Known helgrind false positive**
In certain cases helgrind throws an error; `pthread_cond_{signal,broadcast}: dubious: associated lock is not held by any thread`. This is a known limitation of using helgrind on condition variables and fixable with the use of semaphores, forbidden in this project.
To show this is indeed a false positive and not a design problem, run the following commands after executing `make`, demonstrating a heavy-contention scenario:

```bash
# run valgrind to catch error:
for i in 1 2 3 4 5; do valgrind --tool=helgrind ./codexion 5 3000 200 200 200 10 800 edf; echo ---; done
# observe that the error occurs after burnout

# run command normally - there should be no burnout at all:
for i in 1 2 3 4 5; do ./codexion 5 3000 200 200 200 10 800 edf; echo ---; done
```



### Blocking cases handled

**Deadlocks in concurrent programming** occur in any situation where none of the members (threads) can proceed to their task, because each is waiting on another one completing their task. The four **Coffman Conditions** are those that if present simultaneously in a system, cause a deadlock. As such addressing only one of these is sufficient to avoid a deadlock. 

| Condition | Status | How |
| --- | --- | --- |
| Mutual Exculsion | Present (Inherent) | Dongles are exclusive resources by design: equal number of dongles & coders, 2 dongles needed per coder. |
| Hold & Wait | **Eliminated** | A coder can only acquire dongles if both are available, otherwise they wait.|
| No Preemption | Present (Not eliminated) | A dongle held by a coder is never forcibly removed from them. |
| Circular Wait | **Eliminated** | Dijsktra's Resource Hierarchy ensures always the lower-indexed dongle gets picked up first, thus breaking circular wait. |

Additional measures have been implemented; including:
- Table with only one coder handled separately as a special case.
- Each dongle manages its requests through a **priority queue** implemented with a mini-heap. Priorities are determined by the given arbitration policy **FIFO** (first in first out) or **EDF** (earliest deadline first). A tie-breaker sequencing is implemented to make the arbitration policy deterministic. This, together with the use of a condition variable, means coders block instead of busy-waiting while another coder holds the resource — directly preventing **starvation**.
- Dongles' cooldown after release is enforced using **timed waits** so threads do not immediately retry and starve each other.
- **Burnout detection** is handled by a dedicated monitor thread sweeping at a tight interval (3ms), with the burnout timestamp captured at the exact moment of detection — not at print time — to stay within the required 10ms precision window.
- **Log serialization** is enforced by a single print mutex, so concurrent `print_log` calls from different coder threads never interleave mid-line.
- The simulation stops cleanly when a coder burns out or when the required number of compiles is reached. In either case, waiting threads are awakened with broadcasts so they can exit safely rather than waiting out a full timeout.

### Thread Synchronization Mechanisms

**Race condition** — occurs when two or more threads access shared, mutable data concurrently, with at least one write involved, and the result depends on unpredictable execution ordering. The following primitives are each implemented to prevent a specific instance of race condition.

#### Simulation-level (`t_sim`)

| Primitive | Guards | Used by |
|---|---|---|
| `print_lock` (mutex) | Console output | Every `print_log` so lines never interleave mid-write |
| `sim_lock` (mutex) | `stop_sim` flag, shared setup state | Monitor (write), all coder threads (read) |
| `s_cond` (cond var) | Synchronized start | Coder threads wait until all coders are fully initialized before beginning |

#### Per-dongle (`t_dongle`)

| Primitive | Guards | Used by |
|---|---|---|
| `d_lock` (mutex) | `is_available`, `last_release`, the dongle's request heap | Any coder requesting/releasing the dongle |
| `d_cond` (cond var) | Availability change | Coders block via `pthread_cond_wait`/`timedwait` instead of busy-polling; woken via `pthread_cond_broadcast` on release or stop_sim |

This pairing implements a **custom priority-aware wakeup**, not a plain wait/signal: on wake, a coder re-checks whether it is the minimum-priority entry in the dongle's own heap before proceeding — broadcast wakes everyone, but only the rightful next holder (per FIFO/EDF ordering) actually takes the dongle; everyone else re-sleeps.

#### Per-coder (`t_coder`)

| Primitive | Guards | Used by |
|---|---|---|
| `lock` (mutex) | `last_compile`, `nb_compiles` | The coder's own thread (write), monitor thread (read, once per sweep) |

#### Monitor thread

A dedicated thread is created after all coders are initialized, sweeping all coders every 3ms to check
- (a) burnout — `now - last_compile > time_to_burnout` and
- (b) whether every coder has reached `nb_compiles_req`.
On first condition it prints the burnout message, using the timestamp captured at the exact moment of detection. After this, or in case of second condition immediately on notice, it calls `stop_sim`, which broadcasts every dongle's `d_cond` so blocked coder threads exit promptly rather than waiting out a full timeout.

#### Race conditions prevented, concretely

| Risk | Mechanism |
|---|---|
| Two coders both grab the same dongle | `d_lock` serializes access; only one can hold it at a time |
| Coder reads stale `last_compile` mid-write by another thread | `coder->lock` |
| Monitor reads a coder's fields while that coder's thread writes them | `coder->lock` held on both sides |
| Garbled/interleaved console output | `print_lock` |
| Busy-waiting on a resource that isn't free yet | `pthread_cond_wait`/`timedwait` instead of polling |
| Immediate re-grab of a dongle before cooldown elapses | `last_release` timestamp checked against `dongle_cooldown` before granting |
| One coder perpetually losing dongle contention (starvation) | Priority-ordered heap (FIFO arrival order or EDF deadline) guarantees the longest-waiting/most-urgent request wins, not arbitrary OS wakeup order |


## Resources
- [The Dining Philosophers problem and different ways of solving it](https://zerobone.net/blog/cs/dining-philosophers-problem/)
- [Dining Philosophers Problem](https://eng.libretexts.org/Courses/Delta_College/Operating_System%3A_The_Basics/06%3A_Deadlock/6.4%3A_Dining_Philosopher_Problem)
- [Threads, Mutexes and Concurrent Programming in C](https://www.codequoi.com/en/threads-mutexes-and-concurrent-programming-in-c/)
- [POSIX Threads (Pthreads): Creation and Termination](https://circuitlabs.net/posix-threads-pthreads-creation-and-termination/)
- [Pthreads: Mutexes for Protecting Critical Sections](https://circuitlabs.net/pthreads-mutexes-for-protecting-critical-sections/)
- [Pthreads: Condition Variables for Signaling Between Threads](https://circuitlabs.net/pthreads-condition-variables-for-signaling-between-threads/)
- [Helgrind: a thread error detector](https://courses.cs.vt.edu/cs3214/fall2011/projects/valgrind/valgrind-3.7.0/docs/html/hg-manual.html)
- [Peer solution](https://github.com/browser22nd/codexion)

### AI usage

AI (Claude) was used as a tutoring aid throughout development — for conceptual explanations, especially putting information from various resources into workable roadmap, targeted code review to catch bugs (pointer misuse, race conditions, lock-ordering issues), and guided debugging sessions (GDB usage tutorial, tracing timing bugs).

Gemini was used to understand and ultimately rectify a particular Helgrind "dubious" error on earlier versions.

All design decisions, struct layouts, and final code were written and understood by the author; AI was not used to generate solutions.