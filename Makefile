CC=clang
CFLAGS=-Wall -Wextra -pedantic -std=c11 -Iinclude
OUT_NAME=Minesweeper
LIBS=-Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

main: main.c
	$(CC) $(CFLAGS) -o $(OUT_NAME) main.c $(LIBS)

run: main.c
	$(CC) $(CFLAGS) -o $(OUT_NAME) main.c $(LIBS)
	./$(OUT_NAME)
	rm $(OUT_NAME)

clean: $(OUT_NAME)
	rm -f $(OUT_NAME)