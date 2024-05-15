# inputs and outputs
CC    = mpicc
SRC  ?= greater_than_k.c
OUT  ?= $(basename $(SRC))
FILE ?= input.txt

# number of processes
NP  ?= 2

# compilation flags
FLAGS = -Wall -Wextra -pedantic -O3

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(OUT)

.PHONY: all clean build run format lint

build: format lint all

clean:
	rm -f $(OUT)

run:
	mpirun -np $(NP) $(OUT) $(FILE)

# dependecies: clang-format
format:
	clang-format -i $(SRC)

# dependencies: cppcheck
lint:
	cppcheck $(SRC)

