#!/bin/bash

../../c-chess-cli/c-chess-cli \
    -each tc=20+0.3 \
    -engine cmd=../src/chai_v3.4.2_debug \
    -engine cmd=../src/chai_v3.4.2_debug \
    -openings file=../scripts/UHO_2022_8mvs_big_+110_+139.epd order=random \
    -repeat \
    -rounds 1 \
    -concurrency 1 \
    -pgn ./res.pgn 1 \
    -log
