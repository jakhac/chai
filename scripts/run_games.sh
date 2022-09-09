#!/bin/bash

eng1="$1"
eng2="$2"

echo "Compare engines" ${eng1} ${eng2}

cutechess-cli -engine name=${eng1} cmd=${eng1} -engine name=${eng2} cmd=${eng2} \
    -each proto=uci \
    tc=20+0.3 \
    -openings file=./8moves_GM_LB.pgn order=random \
    -rounds 5000 \
    -concurrency 3 \
    -ratinginterval 1 \
    -outcomeinterval 1 \
    -recover \
    -repeat \
    -pgnout ./res.pgn
