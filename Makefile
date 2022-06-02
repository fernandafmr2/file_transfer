CC=gcc
CFLAGS= -Wall -Wextra

all: receive send


receive: receive.c
	$(CC) $(CFLAGS) -o receive receive.c

send: send.c
	$(CC) $(CFLAGS) -o send send.c

clean:
	rm -rf *.dYSM receive send