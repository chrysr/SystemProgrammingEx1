CC=g++
CFLAGS=-I.
DEPS=tree.h hashtable.h list.h functions.h hashtable.h
OBJ=main.o tree.o hashtable.o list.o functions.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bitcoin: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.Phony: clean
clean:
	-rm $(OBJ)
	-rm bitcoin
