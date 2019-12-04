2048:2048.o main.o
	gcc -g -o 2048 2048.o main.o
2048.o: 2048.c 2048.h
	gcc -g -c 2048.c
main.o: main.c 2048.h
	gcc -g -c main.c
clean:
	rm -rf 2048
	rm -rf *.o