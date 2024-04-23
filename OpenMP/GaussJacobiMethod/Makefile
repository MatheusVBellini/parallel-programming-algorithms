# inputs and outputs
IN_PAR = jacobipar.c
IN_SEQ = jacobiseq.c
OUT_PAR = jacobipar
OUT_SEQ = jacobiseq

IN = $(IN_PAR) $(IN_SEQ)
OUT = $(OUT_PAR) $(OUT_SEQ)

ZIP= ParallelGaussJacobi.zip

# compilation flags
FLAGS= -Wall -Wextra -pedantic

all: seq par

build: format lint all

clean:
	rm -f $(OUT) $(ZIP)

zip:
	zip -r $(ZIP) * -x $(OUT)

seq:	
	gcc $(FLAGS) -o $(OUT_SEQ) $(IN_SEQ)

par:	
	gcc $(FLAGS) -o $(OUT_PAR) $(IN_PAR) -fopenmp

# dependecies: clang-format
format:
	clang-format -i $(IN)

# dependencies: cppcheck | splint
lint:
	cppcheck $(IN)
