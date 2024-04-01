CC = gcc
CFLAGS = -Wall -Wextra -g

.PHONY: all server client clean

all: server client

ifeq ($(BLD_FLAG),s)
server: tcp_server
else ifeq ($(BLD_FLAG),c)
client: tcp_client
else
server: tcp_server
client: tcp_client
endif

tcp_server: server/tcp_server.c server/tcp_server.h common/tcp_common.h
	$(CC) $(CFLAGS) -o $@ server/tcp_server.c

tcp_client: client/tcp_client.c common/tcp_common.h
	$(CC) $(CFLAGS) -o $@ client/tcp_client.c

clean:
	rm -f tcp_server tcp_client
