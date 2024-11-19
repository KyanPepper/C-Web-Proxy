CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

all: proxy

proxy: main.c proxy.c
	$(CC) $(CFLAGS) -o proxy main.c proxy.c


clean:
	rm -f proxy *.o *~