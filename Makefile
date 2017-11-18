test: main.c io.h io.c number.h number.h
	gcc main.c io.c number.c -o $@
