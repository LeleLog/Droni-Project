

IDIR=../../../con2redis/src
ODIR=../obj
BDIR=../bin

CC=g++
CFLAGS=-std=c++20 -g -ggdb -fpermissive -L/usr/local/lib -Wall -I. -I$(IDIR) -lpq -lm -lhiredis

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2redis_OBJ=$(wildcard ../../../con2redis/obj/*.o)


all:	con2redis $(BDIR)/main

con2redis:
	$(MAKE) -C ../../../con2redis/src


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(con2redis_OBJ)
	$(CC) -o $@ $(OBJ) $(con2redis_OBJ) $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 
