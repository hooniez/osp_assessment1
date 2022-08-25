CC=g++

all: Task1Main.o
	$(CC) -Wall -Werror -std=c++14 -o Task1Filter Task1Main.o

Task1Main.o: Task1Main.cpp Task1Filter.cpp
	g++ -c Task1Main.cpp Task1Filter.cpp

clean:
	rm *.o Task1Filter
