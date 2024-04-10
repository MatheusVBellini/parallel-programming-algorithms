FILE ?= default.c
OUT = main

all: $(OUT)

$(OUT): $(FILE)
	gcc -o $@ $^ -pthread

clean:
	rm -f $(OUT)
