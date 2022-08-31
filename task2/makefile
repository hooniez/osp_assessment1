CC=g++

all: Task2.o
	$(CC) -O2: -g -Wall -Werror -std=c++14 -o Task2 Task2.o

Task2.o: Task2.cpp ../task1/Task1Filter.cpp
	$(CC) -O2 -g -c Task2.cpp ../task1/Task1Filter.cpp

clean:
	rm *.o Task2;