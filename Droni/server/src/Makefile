

#IDIR=../hdr
IDIR= ../../../con2redis/src
ODIR= ../obj
BDIR= ../bin

DBIDIR = ../../../con2db


CC=g++
# use this when connecting to Postgresql (library lpq)
#CFLAGS=-std=c++11 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm 
CFLAGS=-std=c++20 -g -ggdb -fpermissive -Wall -I. -I$(IDIR) -I$(DBIDIR) -I/usr/include/postgresql -lpq -lm -lhiredis

# use this when no connection to Postgresql (library lpq) is needed
#CFLAGS=-std=c++11 -I. -I$(IDIR) -lm 

DEPS = $(wildcard $(IDIR)/*.h) $(wildcard $(DBIDIR)/*.h) 
#OBJ = $(wildcard $(ODIR)/*.o)
SRC = $(wildcard *.cpp) 
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2db_OBJ = $(wildcard ../../../con2db/*.o)
con2redis_OBJ=$(wildcard ../../../con2redis/obj/*.o)
client_OBJ = $(wildcard ../../client/obj/drone.o)


all:	con2redis con2db $(BDIR)/main



con2redis:
	$(MAKE) -C ../../../con2redis/src

con2db:
	$(MAKE) -C ../../../con2db


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(client_OBJ) 
	$(CC) -o $@ $(OBJ) $(con2db_OBJ) $(con2redis_OBJ) $(client_OBJ) $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 
	
	
