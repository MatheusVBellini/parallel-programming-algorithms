#!/bin/zsh

# compile
make all > /dev/null 2>&1

# execution schedule
Ns=(1000 5000 10000)
Ts=(2 4 8)
exec_num=30

# sequential execution
test_list=(5 10 50)
for N in "${test_list[@]}"; do
  echo "[\"SEQ-N$N\"]" > "./logs/SEQ-N$N.txt"                   # create log file to save time values
  for ((i = 1; i <= exec_num; i++)); do
    random_seed=$(( (RANDOM % 1000) + 1 ))                      # generate random seed for the code
    random_choice=$(( RANDOM % $N ))                            # generate random choice for input
    t=$({ time ./jacobiseq "$N" 1 "$random_seed" <<< "$random_choice"; } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')  # returns only turnaround time
    echo "$t" >> "./logs/SEQ-N$N.txt"
  done
done

# clean folder
make clean > /dev/null
