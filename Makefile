CC = g++
CFLAGS  = -std=c++11

OBJECTS = HogLogic.o main.o

all: Quidditch

Quidditch: $(OBJECTS)
	$(CC) $(CFLAGS) -o Quidditch $(OBJECTS)

HogLogic.o: HogLogic.cpp HogLogic.hpp
	$(CC) $(CFLAGS) -c HogLogic.cpp

main.o: main.cpp HogLogic.hpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	$(RM) Quidditch $(OBJECTS)