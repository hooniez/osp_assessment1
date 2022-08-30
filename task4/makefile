CC=g++

all: Task4.o
	$(CC) -O2 -Wall -Werror -std=c++14 -pthread -o Task4 Task4.o

Task4.o: Task4.cpp ../task1/Task1Filter.cpp
	$(CC) -O2 -c Task4.cpp ../task1/Task1Filter.cpp

clean:
	rm *.o Task4;