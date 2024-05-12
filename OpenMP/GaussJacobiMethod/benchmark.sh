#!/bin/zsh

# compile
make all > /dev/null 2>&1

# execution schedule
Ns=(1000 5000 10000)
Ts=(2 4 8)
exec_num=30

# sequential execution
for N in "${Ns[@]}"; do
  seq_file="./logs/SEQ-N$N.txt"
  if [[ -f "$seq_file" ]]; then
    echo "Skipping sequential execution for N=$N (log file already exists)"
  else
    echo "Starting sequential execution for N=$N"
    echo "[\"SEQ-N$N\"]" > "$seq_file"                          # create log file to save time values
    for ((i = 1; i <= exec_num; i++)); do
      random_seed=$(( (RANDOM % 1000) + 1 ))                    # generate random seed for the code
      random_choice=$(( RANDOM % $N ))                          # generate random choice for input
      t=$({ time ./jacobiseq "$N" 1 "$random_seed" <<< "$random_choice" -p -t; } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      # returns only turnaround time
      echo "$t" >> "$seq_file"
    done
  fi
done

# parallel execution
for T in "${Ts[@]}"; do
  for N in "${Ns[@]}"; do
    par_file="./logs/PAR-T$T-N$N.txt"
    if [[ -f "$par_file" ]]; then
      echo "Skipping parallel execution for T=$T, N=$N (log file already exists)"
    else
      echo "Starting parallel execution for T=$T, N=$N"
      echo "[\"PAR-T$T-N$N\"]" > "$par_file"                     # create log file to save time values
      for ((i = 1; i <= exec_num; i++)); do
        random_seed=$(( (RANDOM % 1000) + 1 ))                    # generate random seed for the code
        random_choice=$(( RANDOM % $N ))                          # generate random choice for input
        t=$({ time ./jacobipar "$N" "$T" "$random_seed" <<< "$random_choice" -p -t; } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')  # returns only turnaround time
        echo "$t" >> "$par_file"
      done
    fi
  done
done

# clean folder
make clean > /dev/null
