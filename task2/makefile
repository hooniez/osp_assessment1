CC=g++

all: Task2.o
	$(CC) -m64 -fno-omit-frame-pointer -g -Wall -Werror -std=c++14 -o Task2 Task2.o

Task2.o: Task2.cpp ../task1/Task1Filter.cpp
	$(CC) -m64 -fno-omit-frame-pointer  -g -c Task2.cpp ../task1/Task1Filter.cpp

clean:
	rm *.o Task2;