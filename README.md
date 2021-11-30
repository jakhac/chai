# chai - Chess Engine

chai is a free [UCI](http://wbec-ridderkerk.nl/html/UCIProtocol.html) compatible chess engine playing at estimated ~2300 elo. In order to play or test this engine, a GUI like [Arena](http://www.playwitharena.de/), [Lucas Chess](https://lucaschess.pythonanywhere.com/) or a command-line interface like [c-chess-cli](https://github.com/lucasart/c-chess-cli) is recommended.

---

## Installation

### Prebuild Binaries

Latest prebuild binaries compiled for Win64 operating systems are provided under [releases](https://github.com/jakhac/chai/releases).

###  Building From Source

The Makefile supports a customizable number of threads and hashtable size as *threads* and *hashMb* parameters. The default values are *hashMb=256* and *threads=available_cores - 1* to allow a spare core for os-related tasks. Note that the number of threads include the main process and is limited by the number of available cores on the current system.

**Build commands:**
```
$ git clone https://github.com/jakhac/chai
$ cd chai/src
$ make help
$ make release [threads=#threads] [hashMb=#hashtableSize]
```

---

## Testing and Strength

The latest version (v2.6.6) is mainly tested with self-play and occasionally with additional engines provided by [CCRL](https://ccrl.chessdom.com/ccrl/404/) for better reference. The time controls are mostly 20/0.3 and 40/0.4.

Version 2.6.6 is playing at estimated ~2300 on 20/0.3 time controls using a 3-core setup and *256Mb* for the hashtable. A build using 4-cores and *1024Mb* increases elo by a small margin.

---

## Features

### UCI Options

The engine implements the UCI protocol and contains the following options:

- Adjust the size of the hashtable in Mb

```
setoption name Hash value <value>
```

- Set number of threads

```
setoption name Threads value <value>
```

- Enable EGTB by providing path to Syzygy EG-Files: 

```
setoption name SyzygyPath value <path/to/egtb_files>
```

### Engine Features

An excerpt of applied techniques and heuristics is listed in the following:

- Search
  - Multi-threaded iterative deepening framework based on [Lazy SMP](https://www.chessprogramming.org/Lazy_SMP)
  - Principal variation search
  - Late move Reductions
  - Transposition Table (4 buckets: replace entry with lowest depth)
- Move Generation
  - Bitboard based move generation with [Magic-Bitboards](http://pradu.us/old/Nov27_2008/Buzz/research/magic/Bitboards.pdf)
  - Special generators for quiescence and check evasions
  - Checker generator (generates all checking moves for first ply of quiescence)
  - Contains Perft option to debug move generator
- Endgame Tablebase Probing ([Fathom](https://github.com/jdart1/Fathom) probing tool)
- Test-Suite based on GoogleTest-Framework (for usage see [here](https://github.com/google/googletest))

---

## Acknowledgement

Following sources supported and influenced development:

- [Stockfish](https://github.com/official-stockfish/Stockfish), [Ethereal](https://github.com/AndyGrant/Ethereal), [Jazz](https://www.chessprogramming.org/Jazz)
- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
- [CCRL reference engines](http://ccrl.chessdom.com/ccrl/404/)
