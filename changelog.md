**Settings**
- 500 Games against each opponent
- Time: 40/0.4
- Opponents: TSCP181, Bubble
 
**Change log from v2.0 (30.12.20)**
<!-- - Results
  - chai - TSCP181 &rarr; *241-189-70* (+35)
  - chai - Bubble &rarr; *108-337-55* (-173) -->
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
<!-- - Results
  - chai - TSCP181 &rarr; 252-187-61 (+49)
  - chai - Bubble &rarr; 92-342-66 ( -191) -->
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
- [x] Compiler Optimizations **DISABLED IN DEVELOPMENT**
- Quiescence
  - [x] Forced checkmates are detected in quiescence
  - [x] Generate checkers at first ply
- Transposition Table
  - [x] Prefetch Pawn Table
  - [ ] Buckets
  - [ ] Refactor usage of ttable hits

# Todo

- ~~Compiler optimizations~~
- Move Generation
  - ~~`scoreMoves()` instead of scoring while generating~~
  - run move gen after hash move failed -> pvs still possible?
  - ~~check evasion~~
  -  ~~quiesence move gen (captures, promotions, check evasions)~~
  - add checks in first ply
  - ~~special see function with early exit for captures like PxQ~~
  - ~~Early return inCheck function is fastest and most reliable, do not use isLegal or leaesKingInCheck~~
    - ~~faster isCheck method? instead of inCheck() -> only check pinned pieces / king attacks of last move~~

- Project
  - clean up utils, attacks, helper bitboards
  - detect endgames (knight, bishop endgame, light/dark squared bishop)
  - try hash move before generating moves?
  - update typedefs, src, structs, key_t, ...
  - Error handler for invalid FEN

- Alpha Beta
  - adaptive null move pruning -> drop into qui
  - mate distance pruning
  - delta pruning
  - futility pruning / reverse null move pruning
  - razoring
  - tt probing
  - aspiration windows
  - iid if no hash move
  - pvs
  - late move reductions
  - see reductions
  - ~~killer moves~~
  - ~~matekiller moves~~
  - ~~counter moves~~
  - ~~history heuristic~~

- Quiesence
  - see pruning
  - hash table with negative depth in qsearch
  - Add checkers in first quiesence ply

- Hash table
  - ~~prefetch table instruction~~
  - buckets
  - rework table probing (in search)
  - hash table with negative depth in qsearch

- Evaluation
  - lazy eval for qsearch or pruning areas
  - more heuristics
  - traps, pins
  - (texel) tuning

- Maybe
  - Lazy SMP
  - Endgame tablebase
