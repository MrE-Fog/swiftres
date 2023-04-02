CC=g++
CFLAGS=-Wall -Wextra -Wpedantic -O3 -std=c++11

all: swiftres

swiftres: swiftres.cpp
        $(CC) $(CFLAGS) swiftres.cpp -o swiftres

clean:
        rm -f swiftres
