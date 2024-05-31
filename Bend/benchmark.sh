#!/bin/zsh

file_name="reduction_sum"
exec_num=30

# Bend - Rust runtime
file="./logs/bend_rust.txt"
prompt="BEND_RUST"
if [[ -f "$file" ]]; then
  echo "Skipping $prompt test (log file already exists)"
else
  echo "Starting execution for $prompt"
  echo "$prompt" > "$file"
  for ((i = 1; i <= exec_num; i++)); do
    t=$({ time bend run "$file_name".bend  } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      
    echo "$t" >> "$file"
  done
fi

# Bend - C runtime
file="./logs/bend_c.txt"
prompt="BEND_C"
if [[ -f "$file" ]]; then
  echo "Skipping $prompt test (log file already exists)"
else
  echo "Starting execution for $prompt"
  echo "$prompt" > "$file"
  for ((i = 1; i <= exec_num; i++)); do
    t=$({ time bend run-c "$file_name".bend  } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      
    echo "$t" >> "$file"
  done
fi

# Bend - CUDA runtime
file="./logs/bend_cuda.txt"
prompt="BEND_CUDA"
if [[ -f "$file" ]]; then
  echo "Skipping $prompt test (log file already exists)"
else
  echo "Starting execution for $prompt"
  echo "$prompt" > "$file"
  for ((i = 1; i <= exec_num; i++)); do
    t=$({ time bend run-cu "$file_name".bend  } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      
    echo "$t" >> "$file"
  done
fi

# C - Default
gcc -o exec -O2 "$file_name".c 
file="./logs/c_default.txt"
prompt="C_DEFAULT"
if [[ -f "$file" ]]; then
  echo "Skipping $prompt test (log file already exists)"
else
  echo "Starting execution for $prompt"
  echo "$prompt" > "$file"
  for ((i = 1; i <= exec_num; i++)); do
    t=$({ time ./exec } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      
    echo "$t" >> "$file"
  done
fi
rm exec

# C - OpenMP
gcc -o exec -O2 "$file_name"_openmp.c -fopenmp
file="./logs/c_openmp.txt"
prompt="C_OPENMP"
if [[ -f "$file" ]]; then
  echo "Skipping $prompt test (log file already exists)"
else
  echo "Starting execution for $prompt"
  echo "$prompt" > "$file"
  for ((i = 1; i <= exec_num; i++)); do
    t=$({ time ./exec } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      
    echo "$t" >> "$file"
  done
fi
rm exec

# Python
file="./logs/python.txt"
prompt="PYTHON"
if [[ -f "$file" ]]; then
  echo "Skipping $prompt test (log file already exists)"
else
  echo "Starting execution for $prompt"
  echo "$prompt" > "$file"
  for ((i = 1; i <= exec_num; i++)); do
    t=$({ time python3 "$file_name".py  } 2>&1 3>&1 >/dev/null | grep -oP '(?<= )[0-9]+\,[0-9]+(?= total)')      
    echo "$t" >> "$file"
  done
fi

