#!/bin/zsh

# compile
make all > /dev/null 2>&1

# execution schedule
Ns=(100 1000 10000)
Ts=(2 4 8)
exec_num=30

# sequential execution
{ time ./jacobiseq 10000 4 4 <<< 0; } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)'  # returns only turnaround time

# clean folder
make clean > /dev/null
