# chai - Chess Engine

_chai_ is an open source UCI compatible chess engine. The engine is still under development but latest versions including source code, a complete changelog (containing test results) and binaries can be found in [releases](https://github.com/jakhac/chai/releases).

## Usage

Since _chai_ implements the [UCI protocol](http://wbec-ridderkerk.nl/html/UCIProtocol.html), the engine could be used like this

```
> uci
id name chai Chess Engine
id author chai
uciok
> isready
... init logging
isreadyok
> ucinewgame
... set up new game from starting position
> go depth 10
... search starts here
```

## Strength

Version 2.4 is playing at estimated 2000 elo according to multiple test runs with engines from [CCRL](https://ccrl.chessdom.com/ccrl/4040/). Time controls vary from 40/0.4 to 20/0.3 for quicker results.

## Search Features

_chai_ uses following techniques and heuristics

- Search
  - PVS alpha-beta search and quiescence search
  - Late move reductions (included in upcoming release)
  - Iterative deepening framework
  - Internal iterative deepening
  - Null move pruning
  - SEE pruning with depth-dependent threshold
  - Futility Pruning at child and parent node
  - Transposition table (4 buckets, replace entry with lowest depth)
  - Pawn hashtable
  - Mate distance pruning
  - Stand-Pat pruning
- MoveGen
  - Bitboard based move generation with magic bitboards
  - Move encoding in 16 bits
  - Special move generator for quiescence (captures and promotions only)
  - Check evasion generator
  - Checker generator (generates all checking moves for first ply of quiescence)
  - Move ordering:
    - Hash Move
    - Good captures (SEE >= 0) with MVV-LVA
    - Promoting Captures
    - Promotions
    - Mate killer moves
    - 2 killer moves
    - Quiet moves (ordered by history heuristic)
    - Losing captures
  - Contains `dividePerft()` option to debug move generator
- Evaluation (only rudimentary, planned to rewrite in version 3.x)
  - Interpolated piece-square-table
  - piece count
  - Basic king safety
  - Evaluation of pawn structure

## Acknowledgement

Following sources supported and influenced development of _chai_ chess engine

- [Stockfish](https://github.com/official-stockfish/Stockfish), [Ethereal](https://github.com/AndyGrant/Ethereal), [Jazz](https://www.chessprogramming.org/Jazz)
- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
