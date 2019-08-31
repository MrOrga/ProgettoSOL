CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -g -Wmissing-field-initializers -D_POSIX_C_SOURCE=200809L -fsanitize=address -fno-omit-frame-pointer

.PHONY : clean test testsum sig

all: server utils libobjstore client

server: server.c worker.c handlermessage.c utils.c signalhandler.c
	@echo "Compilazione server"
	$(CC) $(CFLAGS) $^ -o $@.o -lpthread

utils: utils.c
	@echo "Compilazione utils"
	$(CC) $(CFLAGS) $^ -c -o $@.o

libobjstore: libobjstore.c
	@echo "Compilazione Libreria"
	$(CC) $(CFLAGS) $^ -c -o $@.o
	ar rvs $@.a $@.o utils.o

client: utils.c client.c
	@echo "Compilazione Client"
	$(CC) $(CFLAGS) $^ -o $@.o  -L . -lobjstore

clean:
	@echo "Pulizia"
	-rm -f *.a
	-rm -f *.o
	-rm -f *.log
	-rm -f *.sock
	-rm -rf data

test:
	@bash test.sh

testsum:
	@bash testsum.sh

sig:
	killall -10 server.o
