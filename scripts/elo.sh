#!/bin/bash  

RESULTS_FILE="/mnt/f/SmartGit-Repositories/chai/archive/results.pgn"
PGN_PATH="/mnt/c/Program Files (x86)/Arena/Tournaments"

cd "$PGN_PATH"
FILES=$(ls -1t | egrep '\.pgn$' | head -3)

touch "$RESULTS_FILE"
echo "" > "$RESULTS_FILE"

# echo "start cat"
for i in $(ls -1t | egrep '\.pgn$' | head -3); do 
    echo $(basename $i) "..." $(date -d "@$(stat -c '%Y' $(basename $i))" '+%c')
    cat $(basename $i) >> "$RESULTS_FILE"
done

echo ""
echo "result stored in " "$RESULTS_FILE"
