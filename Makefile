HASHTABLE=./hash_table
LIST=./list
MEMORY=./memory
REPLACEMENTALGORITHMS=./replacement_algorithms


CC=gcc
CFLAGS=-g -Wall -I$(HASHTABLE) -I$(LIST) -I$(MEMORY) -I$(REPLACEMENTALGORITHMS)

OBJ1=main.o $(HASHTABLE)/hash_table.o $(LIST)/list.o $(MEMORY)/memory.o $(REPLACEMENTALGORITHMS)/replacement_algorithms.o

EXEC1=simulator

all: $(EXEC1)

$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $(EXEC1)


.PHONY: clean

clean:
	rm -f $(OBJ1) $(EXEC1)
