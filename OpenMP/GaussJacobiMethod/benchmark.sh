#!/bin/zsh

# compile
make all > /dev/null 2>&1

# sequential execution
{ time ./jacobiseq 1000 4 4 <<< 0; } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)'

# clean folder
make clean > /dev/null
