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

- Results
  - Chai_2.2_1bucket - Chai_2.2_2bucket &rarr; 59%: 247-157-96 (+63)
  - Chai_2.2_1bucket - Chai_2.2_4bucket &rarr; 75%: 335-86-79 (+191)
  - Chai_2.2_1bucket - Chai_v2.1.2 &rarr; 62%: 252-132-116 (+85)
  - Chai_2.2_1bucket - Tscp181 &rarr; 58%: 253-176-71 (+56)
  - chai (1bucket) - TSCP181 &rarr; 52%: 232-212-56 (+14)
  - chai (1bucket) - Bubble &rarr; 29%: 115-321-64 (-1561)

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

- Elo

  - F-Pruning at frontier nodes

    - chai_v2.4 - chai_v2.3.2 &rarr; 54%: 203-162-135 (+28)
    - chai - TSCP181 &rarr; 64%: 284-143-73 (+100)
    - chai - Bubble &rarr; 30%: 115-311-74 (-147)

  - Enabled beta-cutoff store

    - chai_v2.4 - chai_v2.3.2 &rarr; 65%: 268-116-116 (+108)
    - chai - Bubble &rarr; 41%: 163-258-79 (-63)

  - Reset MateKillers, F-Pruning pre-frontier nodes, Strelka-Razoring, Beta-Cutoff

    - chai_strelka - Bubble &rarr; 37%: 127-236-67 (-92)
    - chai_strelka - chai_v.2.4_fp_cont &rarr; 43%: 139-203-88 (-49)

  - Replaced Strelka-Razoring with conservative margin, Beta-Cutoff

    - chai_v2.4_razor_std - Bubble &rarr; 40%: 307-517-176 (-70)
    - chai_v2.4_razor_std - chai_v.2.4_fp_cont &rarr; 49%: 355-378-267 (-7)

  - F-Pruning frontier and pre frontier nodes, Reset MateKillers, Beta-Cutoff

    - chai_v2.4 - Bubble &rarr; 43%: 342-482-176 (-49)
    - chai_strelka - Bubble &rarr; 34%: 268-586-146 (-115)

  - Added Mate Distance Pruning

    - chai_v2.4 - Bubble &rarr; 44%: 180-242-78 (-42)

**Changes in v2.5**

- AlphaBeta
  - [x] Principal Variation Search
  - [x] Consider `IS_PV` and `NO_PV` in pruning decisions
  - [x] Late Move Reductions
  - [x] Refactor quiescence: Checkmate detection and standPat pruning
  - [x] Interal Iterative Deepening considers node type
  - [x] Check for repetition before probing ttable
- Transposition Table
  - [x] TT probing with index (lower bits) and key (upper bits)
  - [x] Retry different bucket sizes and replacement schemes
  - [ ] Prefetch bucket instead of entry
- Project
  - [x] Replaced `pvLine_t` with `printTTablePV()`
  - [x] Remove `pvLine_t` lines from function signatures
  - [x] UCI currentMove and move number
  - [ ] UCI refactor uci calls (to be executed without order) and cli
  - [x] `alphaBetaRoot()` function for save mate/draw detection and bestMove
  - [x] Templates for `alphaBeta()` and `quiescence()`
  - [ ] Retrieve pv-line from pv-structs
  - [ ] Markdown table in changelog
- Eval
  - [x] Check: insufficient material cannot be influenced by contempt factor
  - [ ] Add `eval_t` for evaluations
- Bugfixes
  - [x] Forced quiescence checkmates used `moveList->cnt` instead of `legalMoves`
  - [x] standPat pruning considers check / not not-check and updates `score, bestScore, alpha` seperately
  - [x] Quiescence check up used `nodes` instead of `qnodes`
  - [x] Disabled `pvLine_t` structs on stack due to memory issues on call-stack
  - [x] `parseFen()` now assigns halfMoves to `fiftyMove`, `isRepetition()` was adapted to this change
  - [x] Bigger margin for `ISMATE` to include long checking sequences from quiescence mate detection
  - [x] Not finding mates: Unforced Draw in winning endgame (fixed with new quiescence mate detection und reordering alphaBeta)
  - [x] Redordered alphaBeta functions till move loop

<!-- - Engines
  - chai_v2.5.2_pv_iid:
    - Root-Call, Old TT, Repetition-Returns, 1B
    - Draws Mate!
  - chai_v2.5.3_basic_rep-return_4b:
    - Basic-Call, New TT, Repetition-Returns, 4B
    - Draws Mate!
  - chai_v2.5.3_root_rep-return_4b:
    - Root-Call, New TT, Repetition-Returns, 4B
    - Draws Mate!
  - chai_v2.5.3_basic_rep-ret-no-root_tt-no_pv:
    - Basic-Call, New TT, Repetition-Returns, 4B
    - No Rep-Return at rootNode, No TT-Cut at pvNode
    - Draw 1 mate in 100 games (might be tt collision or fpruning) -->

- Elo

  - v2.5.0
    - Plain PVS, consider PV nodes in NMP and Futility Pruning
    - chai_v2.5.0 - chai_v2.4 &rarr; 55%: 211-164-125 (+35)
  - v2.5.1
    - Added Late Move Reductions
    - chai_v2.5.1 - chai_v2.4 &rarr; 74%: 307-70-123 (+182)
  - v2.5.2 (= v2.5.2_pv_iid)
    - Final changes:
      - Root-Call, Old TT, Repetition-Returns, 1 buckets
      - Forfeit on Time Bugfix, forced checkmate in quiescence fix
      - Fix 3-fold-repetition with mate in x, alphaBetaRoot function vs root function
    - chai_v2.5.2 - chai_v2.5_std &rarr; 63%: 251-96-243 (+92)
    - chai_v2.5.2_root - chai_v2.5.2_basic &rarr; 50%: 93-93-105 (+-0)
    - chai_v2.5.2 - Bubble &rarr; 73%: 305-93-65 (+173)
    - chai_v2.5.2 - Bumblebee &rarr; 14%: 43-373-47 (-315)
  - v2.5.3 (= v2.5.3_basic_rep-ret-no-root_tt-no_pv)
    - Final changes
      - Basic call, new tt, no rep-return in rootNode, no tt-cutoff in pvNode, 4 buckets
    - chai_v2.5.3_basic_rep-return_4b - chai_v2.5.2_pv_iid &rarr; 178-61-57 (+147)
    - chai_v2.5.3_root_rep-return_4b - chai_v2.5.2_pv_iid &rarr; 41-115-32 (-7)
    - chai_v2.5.3_root_rep-return_4b - chai_v2.5.3_basic_rep-return_4b &rarr; 90-94-3 (-147)
    - chai_v2.5.3_basic_rep-ret-no-root_tt-no_pv - chai_v2.5.2_pv_iid &rarr; 350-92-58 (+200)
    - chai_v2.5.3_basic_rep-ret-no-root_tt-no_pv - chai_v2.5.3_basic_rep-return_4b &rarr; 155-141-204 (+7)
    - chai_v2.5.3 - Bubble &rarr; 73%: 312-97-91 (+164)
    - chai_v2.5.3 - Bumblebee &rarr; 14%: 46-395-59 (-301)
  - v2.5.4

    - Final changes:
      - (Elo) No mate distance pruning in rootNodes, IID extends when check, first move is always PV, bucket prefetch (cachelines?)
      - (Code) Template functions, currmove UCI info, value_t
    - TODO:
      - uci setoptions
      - pvline struct, uci cli (debug lichess-bot)
    - chai_v2.5.4 - chai_v2.5.3 &rarr; 38%: 93-210-197 (-85)
    - chai_v2.5.4_no_prefetch - chai_v2.5.3 &rarr; 40%: 111-214-175 (-70)
    - chai_v2.5.4_pv_var - chai_v2.5.3 &rarr; 49%: 143-158-199 (-7)

    - chai_v2.5.4_lmr2 - chai_v2.5.3 &rarr; 41%: 107-200-193 (-63)
    - chai_v2.5.4_pv_var_lmr2 - chai_v2.5.3 &rarr; 52%: 163-145-192 (+14)

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
  - refactor checkup usage and function

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
  - npm verification
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
