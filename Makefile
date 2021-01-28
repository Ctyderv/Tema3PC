
OBJ = ./src/main.o ./src/jread.o ./src/sdoksolv.o
DEPS = ./src/jread.h ./src/sdoksolv.h
CFLAGS=-Wall -Wextra -Isrc

%.o: %.c $(DEPS)
	gcc -g -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	gcc -g -o sudoku $^ $(CFLAGS)
	
.PHONY: clean run

clean:
	rm ./src/*.o sudoku

run: build
	./sudoku
