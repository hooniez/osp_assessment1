

all: Task3.o
	g++ -O2 -Wall -Werror -std=c++14 -pthread -o Task3 Task3.o

Task3.o: Task3.cpp ../task1/Task1Filter.cpp
	g++ -O2 -c Task3.cpp ../task1/Task1Filter.cpp

clean:
	rm *.o Task3;