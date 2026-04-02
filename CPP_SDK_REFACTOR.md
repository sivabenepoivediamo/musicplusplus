# C++ SDK refactor (header-only)

This note is for anyone maintaining the original **musicplusplus** C++ headers: what changed, and why.

## Goals

- **Snake_case** for file names, types, and most public APIs so the surface feels consistent.
- **`namespace musicpp`** for all library types and free functions.
- **`MUSICPP_*_H`** include guards instead of mixed `POSITIONVECTOR_H` / `Vector_H` style.
- **No `using namespace std`** in public headers; use **`std::`** explicitly.
- **`math_util.h`**: `division_result`, `euclidean_division`, **`gcd`** / **`lcm`** as `inline` functions (replacing the old `static` helpers in a header).
- **Lean includes**: `#include "vectors.h"` with `-I src` from CMake (no `./` prefixes).
- **Unified vectors**: `Vector.h` and the old duplicate `vectors.h` are merged into a single **`vectors.h`** exposing **`vector_set`** plus **`positions_to_intervals`**, **`intervals_to_positions`**, **`positions_to_onset`** as `inline` functions in `musicpp` (one implementation; no drift between class and free helpers).
- **Encapsulation**: pitch containers keep internal `*_` members; use **`data()`**, **`mod()`**, **`offset()`**, **`effective_range()`** (and the other accessors) instead of touching `.data` / `.mod` directly.
- **Italian comments** in rhythm/binary code were translated to English where they remained.
- **Correctness preserved**: Euclidean rhythm still uses the **pattern length** as modulus (`onset_vector::euclidean` returns `mod = static_cast<int>(result.size())`), and **positions → intervals** keeps the **cyclic closure** interval (the old loop read one past the end; that is now explicit and safe).

## Type and API renames (quick map)


| Before                                             | After                                    |
| -------------------------------------------------- | ---------------------------------------- |
| `PositionVector`                                   | `musicpp::position_vector`               |
| `IntervalVector`                                   | `musicpp::interval_vector`               |
| `BinaryVector` / `binary_vector`                   | `musicpp::onset_vector`                  |
| `Vectors`                                          | `musicpp::vector_set`                    |
| `getData()` / `getMod()` / `getRange()` (position) | `data()` / `mod()` / `effective_range()` |
| `getOffset()` / `setOffset()` (interval)           | `offset()` / `set_offset()`              |
| `adaptToLCM`                                       | `adapt_to_lcm`                           |
| `roto_translate` (position & interval)             | `relative_mode`                          |
| interval `rotate`                                  | `parallel_mode`                          |
| position `rotate`                                  | `rotate`                                 |
| `positions_to_binary` / `binary()`                 | `positions_to_onset` / `onset()`         |
| `positionsToIntervals` (free)                      | `positions_to_intervals`                 |
| `DivisionResult` / `euclideanDivision`             | `division_result` / `euclidean_division` |


`vector_set` methods follow the same idea: **`positions()`**, **`intervals()`**, **`onset()`**, **`from_positions`** / **`from_intervals`** / **`from_onset`**, **`rotate`** (positions), **`relative_mode`**, **`parallel_mode`** (intervals), **`update_from_positions`**, etc.

## File layout (`packages/cpp-sdk/src`)

Core: `utility.h`, `math_util.h`, `position_vector.h`, `interval_vector.h`, `onset_vector.h`, `vectors.h`.

Renamed for consistency: `matrix_distance.h`, `scale_dictionary.h`, `note_names.h`, `chord_names.h`, `rhythm_gen.h`, `quantize_transpose.h`.

Removed legacy names: `positionVector.h`, `intervalVector.h`, `binaryVector.h`, `Vector.h`, old split `vectors.h`, `mathUtil.h`.

## Build / tests

CMake still does `target_include_directories(musicplusplus INTERFACE src)`. Consumers qualify types with **`musicpp::`**. All **10** Catch2 targets under `packages/cpp-sdk` were run after the refactor; they pass.

## If you extend the library

- Add new headers under `src/` with **`MUSICPP_…_H`** guards and **`namespace musicpp`**.
- Avoid `using namespace std` in headers.
- Prefer **`inline`** free functions in headers over `static` file-local functions if they must live in a header.

