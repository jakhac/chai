**Settings**

- 500 Games against each opponent
- Time: 40/0.4
- Opponents: TSCP181, Bubble

**Change log from v2.0 (30.12.20)**

- Results

  - chai - TSCP181 &rarr; _241-189-70_ (+35)
  - chai - Bubble &rarr; _108-337-55_ (-173)

- AlphaBeta
  - Standard alpha-beta pruning
  - Check Extensions:
    - Always increment depth by 1 if in check
  - Move Ordering:
    1. Hash Move from TTable
    2. Good captures (caps with SEE > 0)
    3. Promoting pawn with capture
    4. Promoting pawn
    5. Equal captures (caps with SEE == 0)
    6. Both standard killer moves (quiet)
    7. Rest of quiet moves ordered by history heuristic
    8. Losing captures
- Quiesence
  - Delta cutoff
  - Same Move ordering like in AlphaBeta
  - Move Gen includes: caps, proms, full check evasions
- Transposition Table
  - Only used in AlphaBeta search
  - Size = 16MB
  - Hash move used in move ordering

**Change log from v2.1 (03.01.21)**

- Results

  - chai - TSCP181 &rarr; 252-187-61 (+49)
  - chai - Bubble &rarr; 92-342-66 (-191)

- Move Ordering:
  - Promoting captures are sorted higher than normal captures
- Quiesence
  - Removed Delta cutoff
  - `lazySee()` early return in see calculation for quiesence (attackerVal < capVal)
  - Negative see pruning
  - `scoreMovesQuiesence`
  - No stand pat pruning when in check
- Transposition Table
  - Prefetch transposition table
  - Size = 128MB
- Fixes and bugs
  - moveScores in alphaBeta swapped moves but not scores
  - counterMoves, EP moves, castleMoves scored wrong at index
  - assign correct PROMOTION bonus in alphaBeta and qui moveScoring
- Features
  - Choose FEN in perft option

**Changes in v2.2**

- Results
  - Chai_2.2_1bucket - Chai_2.2_2bucket &rarr; 59%: 247-157-96 (+63)
  - Chai_2.2_1bucket - Chai_2.2_4bucket &rarr; 75%: 335-86-79 (+191)
  - Chai_2.2_1bucket - Chai_v2.1.2 &rarr; 62%: 252-132-116 (+85)
  - Chai_2.2_1bucket - Tscp181 &rarr; 58%: 253-176-71 (+56)
  - chai (1bucket) - TSCP181 &rarr; 52%: 232-212-56 (+14)
  - chai (1bucket) - Bubble &rarr; 29%: 115-321-64 (-1561)
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

- Elo
  - ttSize=128MB
    - chai - Chai_2.2_1bucket &rarr; 51%: 201-191-108 (+7)
    - chai - TSCP181 &rarr; 56%: 244-186-70 (+42)
    - chai - Bubble &rarr; 27%: 102-330-68 (-173)
  - ttSize=200MB and reordered
    - chai - Chai_2.2_1bucket &rarr; 49%: 196-184-120 (-7)
    - chai - Bubble &rarr; 27%: 90-320-90 (-173)
  - NMP (At least 7 pieces, d>2), ttSize=256MB, no-logs, `lazyEval()`
    - chai - Chai_2.2_1bucket &rarr; 52%: 202-185-113 (+14)
    - chai - Bubble &rarr; 34%: 123-286-91 (-115)
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

- Elo
  - F-Pruning at frontier nodes
    - chai_v2.4 - chai_v2.3.2 &rarr; 54%: 203-162-135 (+28)
    - chai - TSCP181 &rarr; 64%: 284-143-73 (+100)
    - chai - Bubble &rarr; 30%: 115-311-74 (-147)
  - F-Pruning at frontier nodes and enabled beta-cutoff store
    - chai_v2.4 - chai_v2.3.2 &rarr; 65%: 268-116-116 (+108)
    - chai - Bubble &rarr; 41%: 163-258-79 (-63)
- AlphaBeta
  - [ ] Futility Pruning
  - [ ] Mate Distance Pruning
  - [ ] Razoring
  - [x] ContemptFactor considers drawn piece-combinations
- Project
  - [x] Update typedefs: `key_t`
  - [x] Rename executables according to version
  - [ ] Refactor `Board`-Class
    - Convert `Board` to `struct board_t`
    - Reduce `board_t` size and share huge arrays between instances if possible
- Bugfixes:
  - [x] kingSafetyArray not enough values (8 atk sq possible)

# Todo

- ~~Compiler optimizations~~
- Move Generation

  - ~~`scoreMoves()` instead of scoring while generating~~
  - run move gen after hash move failed -> pvs still possible?
  - ~~check evasion~~
  - ~~quiesence move gen (captures, promotions, check evasions)~~
  - ~~generate quiet checks~~
  - ~~special see function with early exit for captures like PxQ~~
  - ~~Early return inCheck function is fastest and most reliable, do not use isLegal or leaesKingInCheck~~
    - ~~faster isCheck method? instead of inCheck() -> only check pinned pieces / king attacks of last move~~

- Project

  - ~~detect endgames (knight, bishop endgame, light/dark squared bishop)~~
  - ~~update typedefs, src, structs, key_t, ...~~
  - Disable log (and use one fd for programm)
  - Timeman.c: dynamic time management
  - Error handler for invalid FEN
  - clean up utils, attacks, helper bitboards

- Alpha Beta

  - ~~mate distance pruning~~
  - futility pruning / reverse null move pruning
  - razoring
  - ~~tt probing~~
  - aspiration windows
  - ~~iid if no hash move~~
  - pvs
  - late move reductions
  - see reductions
  - try hash move before generating moves
  - ~~reorder fiftyMove, mate distance pruning before quiescence~~
  - ~~killer moves~~
  - ~~matekiller moves~~
  - ~~counter moves~~
  - ~~history heuristic~~

  - nmp drops into qui ?
  - null killer move?

- Quiesence

  - ~~see pruning~~
  - delta pruning
  - hash table with negative depth in qsearch
  - ~~Add checkers in first quiesence ply~~

- Hash table

  - ~~prefetch table instruction~~
  - ~~buckets~~
  - ~~rework table probing (in search)~~
  - probe in qsearch
  - hash table with negative depth in qsearch

- Evaluation

  - more heuristics
  - interpolation with pieces instead of moves
  - traps, pins
  - (texel) tuning

- Maybe
  - Lazy SMP
  - Endgame tablebase
