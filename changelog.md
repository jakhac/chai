**Change log from v2.0 (30.12.20)**

- AlphaBeta
  - [x] Standard alpha-beta pruning
  - [x] Check Extensions:
    - Always increment depth by 1 if in check
  - [x] Move Ordering:
    1. Hash Move from TTable
    2. Good captures (caps with SEE > 0)
    3. Promoting pawn with capture
    4. Promoting pawn
    5. Equal captures (caps with SEE == 0)
    6. Both standard killer moves (quiet)
    7. Rest of quiet moves ordered by history heuristic
    8. Losing captures
- Quiesence
  - [x] Delta cutoff
  - [x] Same Move ordering like in AlphaBeta
  - [x] Move Gen includes: caps, proms, full check evasions
- Transposition Table
  - [x] Only used in AlphaBeta search
  - [x] Size = 16MB
  - [x] Hash move used in move ordering

**Change log from v2.1 (03.01.21)**

- Move Ordering:
  - [x] Promoting captures are sorted higher than normal captures
- Quiesence
  - [x] Removed Delta cutoff
  - [x] `lazySee()` early return in see calculation for quiesence (attackerVal < capVal)
  - [x] Negative see pruning
  - [x] `scoreMovesQuiesence`
  - [x] No stand pat pruning when in check
- Transposition Table
  - [x] Prefetch transposition table
  - [x] Size = 128MB
- Fixes and bugs
  - [x] moveScores in alphaBeta swapped moves but not scores
  - [x] counterMoves, EP moves, castleMoves scored wrong at index
  - [x] assign correct PROMOTION bonus in alphaBeta and qui moveScoring
- Features
  - [x] Choose FEN in perft option

**Changes in v2.2**

- [x] Compiler Optimizations **DISABLED WHILE TESTING v2.2**
- Quiescence
  - [x] Forced checkmates are detected in quiescence
  - [x] Generate checkers at first ply
- Transposition Table
  - [x] Prefetch Pawn Table
  - [x] Buckets
  - [x] Replacement Strategy: Overwrite lowest depth
  - [x] Refactor usage of ttable hits
- Features
- [x] Parse FEN into board from console
- [x] Print FEN
- Bugfixes
- [x] Fixed incorrect eval type (`uint_16`) in pawn entry
- [x] Alpha cutoff stored NO_MOVE in TT

**Changes in v2.3**

- Eval
  - [x] detect endgames (knight, bishop endgame, light/dark squared bishop)
  - [x] `lazyEval()` used in NMP and `contemptFactor()`
- Transposition Table
  - [x] ttSize=256MB
- AlphaBeta
  - [x] Adaptive Null Move Pruning
  - [x] IID if no hash move
  - [x] Reorder: Check for FiftyMove, Mate Distance Pruning (disabled) and TTable before dropping in quiescence
- Project

  - [x] UCI Mate in X option
  - [x] Low time regulations in `uciParseGo()`
  - [x] Removed `log()` in UCI calls

**Changes in v2.4**

- AlphaBeta

  - [x] Futility Pruning
  - [x] Razoring (implemented but was losing elo, retry with )
  - [x] Mate Distance Pruning
  - [x] ContemptFactor considers drawn piece-combinations
  - [x] Reset MateKillers between search

- Project
  - [x] Update typedefs: `key_t`
  - [x] Rename executables according to version
  - [x] Refactor `Board`-Class
    - Convert `Board` to `struct board_t`
    - Reduce `board_t` size and share huge arrays between instances if possible
  - [x] Namespace `Piece::` for piece enums
- Bugfixes:

  - [x] kingSafetyArray not enough values (8 atk sq possible)
  - [x] `ISMATE` considers forced mate in quiescence search (lower `ISMATE` score)

**Changes in v2.5**

- AlphaBeta
  - [x] Principal Variation Search
  - [x] Consider `IS_PV` and `NO_PV` in pruning decisions
  - [x] Late Move Reductions
  - [x] Refactor quiescence: Checkmate detection and standPat pruning
  - [x] Interal Iterative Deepening, reduction style
  - [x] Check for repetition before probing ttable
  - [x] Queen prom only in quiescence
  - [x] Search Stack
- Transposition Table
  - [x] TT probing with index (lower bits) and key (upper bits)
  - [x] Retry different bucket sizes and replacement schemes
  - [x] Prefetch bucket instead of entry
- Project
  - [x] Replaced `pvLine_t` with `printTTablePV()`
  - [x] Remove `pvLine_t` lines from function signatures
  - [x] UCI currentMove and move number
  - [x] UCI refactor calls (to be executed without order) and cli
  - [x] Templates for `alphaBeta()` and `quiescence()`
  - [x] Retrieve pv-line from pv-structs
  - [x] Search Stack
  - [x] 16Bit move encoding
  - [x] Support Makefile
- Eval
  - [x] Check: insufficient material cannot be influenced by contempt factor
  - [x] Add `eval_t` for evaluations
- Bugfixes
  - [x] Forced quiescence checkmates used `moveList->cnt` instead of `legalMoves`
  - [x] standPat pruning considers check / not in-check and updates `score, bestScore, alpha` seperately
  - [x] Quiescence check up used `nodes` instead of `qnodes`
  - [x] Disabled `pvLine_t` structs on stack due to memory issues on call-stack
  - [x] `parseFen()` now assigns halfMoves to `fiftyMove`, `isRepetition()` was adapted to this change
  - [x] Bigger margin for `ISMATE` to include long checking sequences from quiescence mate detection
  - [x] Not finding mates: Unforced Draw in winning endgame (fixed with new quiescence mate detection and reordering alphaBeta)
  - [x] Redordered alphaBeta functions till move loop

**Changes in v2.6**

- Search
  - v2.6.1
    - [x] Delta Pruning
    - [x] Store/Probe ttable in quiescence search
  - v2.6.2
    - [x] Aspiration Windows
  - v2.6.3
    - [x] Support for EGTB Probing
  - v2.6.4
    - [x] archive repository in msvc_branch
    - [x] makefile and g++ code adjustments
    - [x] Multi-Threading
  - v2.6.5
    - [x] Time Management
    - [x] Makefile supports #threads and #hashMb parameters
  - v2.6.6
    - [x] Select best thread
- Project
  - [x] UCI fixes, e.g. quit current mode
  - [x] UCI refactor parsing
  - [x] Update README.md to new build process


**Changes in v3.0**

Evaluation
- v3.0.1
  - [x] Include king position in pawnKey
  - [x] Update PSQT valueand materialBalance on-the-fly
- v3.0.2
  - [x] Use color_t type
  - [x] Sophisticated evaluation of pawn structure (+ king-safety wrt. pawns)
- v3.0.3
  - Refactor evaluation for:
    - [x] King Safety     (v3.0.3 +15)
    - [x] Knight          (v3.0.4 +10)
    - [x] Mobility Terms  (v3.0.5 +5)
    - [x] Bishop          (v3.0.6 +7)
    - [x] Rook            (v3.0.7 +7)
    - [x] Queen           (v3.0.8 +5)
    - [ ] Dedicated endgame evals (KPvK, KBNvK)
- Project
  - [ ] Implement testEGTB file
  - constexpr masks


# Todo

- Move Generation

  - ~~`scoreMoves()` instead of scoring while generating~~
  - ~~run move gen after hash move failed -> pvs still possible?~~
  - ~~check evasion~~
  - ~~quiesence move gen (captures, promotions, check evasions)~~
  - ~~generate quiet checks~~
  - ~~special see function with early exit for captures like PxQ~~
  - ~~Early return inCheck function is fastest and most reliable, do not use isLegal or leaesKingInCheck~~
  - ~~faster isCheck method? instead of inCheck() -> only check pinned pieces / king attacks of last move~~

- Project

  - ~~detect endgames (knight, bishop endgame, light/dark squared bishop)~~
  - ~~update typedefs, src, structs, hashhashKey_t, ...~~
  - Time management (rapid, blitz, bullet mode for lichess)
  - ~~Opening book~~

- Alpha Beta

  - ~~mate distance pruning~~
  - ~~futility pruning / reverse null move pruning~~
  - ~~raozring~~
  - ~~tt probing~~
  - ~~aspiration windows~~
  - ~~iid if no hash move~~
  - ~~pvs~~
  - ~~late move reductions~~
  - ~~see reductions~~
  - ~~npm verification~~
  - ~~reorder fiftyMove, mate distance pruning before quiescence~~
  - ~~killer moves~~
  - ~~matekiller moves~~
  - ~~counter moves~~
  - ~~history heuristic~~

- Quiesence

  - ~~see pruning~~
  - ~~delta pruning~~
  - ~~hash table probing and storing (negative depth in qsearch)~~
  - ~~Add checkers in first quiesence ply~~

- Hash table

  - ~~prefetch table instruction~~
  - ~~buckets~~
  - ~~rework table probing (in search)~~

- Evaluation
  - more heuristics
  - interpolation with pieces instead of moves
  - traps, pins
  - EvalStack psqt
  - (texel) tuning
