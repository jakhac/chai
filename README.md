# chai - Chess Engine

chai is a free [UCI](http://wbec-ridderkerk.nl/html/UCIProtocol.html) compatible chess engine playing at estimated ~2300 elo supporting state-of-the-art NNUE evaluation. In order to play or test this engine, a GUI like [Arena](http://www.playwitharena.de/), [Lucas Chess](https://lucaschess.pythonanywhere.com/) or a command-line interface like [c-chess-cli](https://github.com/lucasart/c-chess-cli) is recommended.



## Installation
---

### Prebuild Binaries

Latest prebuild binaries including different architectures and active NNUE are provided in [releases](https://github.com/jakhac/chai/releases). The engine speed between the architectures is _avx2 > ssse3 > none_. In case you do not know which architecture is supported on your CPU, you can either check with tools like HWINFO and CPUID or simply try to run the binaries (almost all somewhat modern CPUs support AVX2).

###  Building From Source

In order to build from source in Windows, C++17 and Make is required. The target to build the engine is called ``release`` and has the following parameters:
- __evalfile__ Here you can set the path with forward-slashes (!) to a NNUE file that should be included during compilation. Currently, no original NNUE files are offered. Instead, we refer to [this](https://github.com/Matthies/NN) repository where supported networks can be found. Note that nets can also be loaded via UCI protocol as explained below.
- __avx2__ Default is `y`. Set this to `n` if your CPU does not support AVX2.
- __ssse3__ Default is `y`. Set this to `n` if your CPU does not support SSSE3.



```
$ git clone https://github.com/jakhac/chai
$ cd chai/src
$ make release [evalfile=path/to/evalfile.nnue] [avx2=y/n] [ssse3=y/n]
```


## Testing, Strength, NNUE
---

### NNUE

When starting to completely refactor the evaluation of this engine, NNUEs were already state-of-the-art and used by most top engines. Instead of tweaking parameters of an handcrafted evaluation and spending numerous resources to incrementally measure and verify small strength improvements (based on outdated heuristics), a support for NNUE files is implemented in the latest release. However, as own networks are not offered yet, we refer to networks trained by [RubiChess](https://github.com/Matthies/NN) which are supported by chai. In fact, the NNUE engines listed in the table below utilize these nets.

### Elo

The engines are mainly tested by self-play and occasionally checked against additional engines provided by [CCRL](https://ccrl.chessdom.com/ccrl/404/) for better reference. The time controls are mostly 20/0.3 and 40/0.4. Engine testing differs on various machines and therefore, the following data is only an estimation derived from an Intel i5-6500 setup.

<center>

| Engine                	| Elo  	|
|-----------------------	|------	|
| chai_v3.4.0_1CPU      	| 2313 	|
| chai_v3.4.0_4CPU      	| 2438 	|
| chai_v3.4.0_1CPU_nnue 	| 3109 	|
| chai_v3.4.0_4CPU_nnue 	| 3221 	|

</center>



## Features
---

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

- Load a NNUE file 

```
setoption name EvalFile value <path/to/nnue.bin>
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
- Evaluation
  - NNUE evaluation as outlined by Gary Linscott in [nnue-pytorch](https://github.com/glinscott/nnue-pytorch/blob/master/docs/nnue.md). The implementation follows the classical HalfKP Feature-Set that was implemented in Stockfish 13.1.
- Move Generation
  - Bitboard based move generation with [Magic-Bitboards](http://pradu.us/old/Nov27_2008/Buzz/research/magic/Bitboards.pdf)
  - Special generators for quiescence and check evasions
  - Checker generator (generates all checking moves for first ply of quiescence)
  - Contains Perft option to debug move generator
- Endgame Tablebase Probing ([Fathom](https://github.com/jdart1/Fathom) probing tool)
- Test-Suite based on GoogleTest-Framework (for usage see [here](https://github.com/google/googletest))

## Acknowledgement
---
Following sources supported and influenced development:

- [Stockfish](https://github.com/official-stockfish/Stockfish), [Ethereal](https://github.com/AndyGrant/Ethereal), [Jazz](https://www.chessprogramming.org/Jazz), [RubiChess](https://github.com/Matthies/NN)
- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
- [CCRL reference engines](http://ccrl.chessdom.com/ccrl/404/)
