CC=gcc
CFLAGS=-Wall -Wextra -g

LOAD=load_balancer
SERVER=server
CACHE=lru_cache
UTILS=utils

# Add new source file names here:
EXTRA=add/*.c

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(CACHE).o $(UTILS).o $(EXTRA)
	$(CC) $^ -g -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(CACHE).o: $(CACHE).c $(CACHE).h
	$(CC) $(CFLAGS) $^ -c

$(UTILS).o: $(UTILS).c $(UTILS).h
	$(CC) $(CFLAGS) $^ -c

$(EXTRA).o: $(EXTRA).c $(EXTRA).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch