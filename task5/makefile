CC=g++

all: Task5.o
	$(CC) -O0 -g -Wall -Werror -std=c++14 -pthread -o Task5 Task5.o

Task5.o: Task5.cpp ../task1/Task1Filter.cpp
	$(CC) -O0 -g -c Task5.cpp ../task1/Task1Filter.cpp

clean:
	rm *.o Task4;