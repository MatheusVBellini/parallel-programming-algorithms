FILE ?= default.c
OUT = main

all: $(OUT)
	
$(OUT) : $(FILE)
	gcc -o $@ $^ -fopenmp

clean:
	rm -f $(OUT)
