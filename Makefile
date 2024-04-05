CC = gcc
CFLAGS = -Wall -Wextra -g -std=gnu99
LDFLAGS = -pthread -static

.PHONY: all server client clean

all: server client

ifeq ($(BLD_FLAG),s)
server: tcp_server
common: tcp_common
else ifeq ($(BLD_FLAG),c)
client: tcp_client
common: tcp_common
else ifeq ($(BLD_FLAG),a)
server: tcp_server
client: tcp_client
common: tcp_common
endif

tcp_server: server/tcp_server.c server/tcp_server.h common/tcp_common.c common/tcp_common.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ server/tcp_server.c common/tcp_common.c

tcp_client: client/tcp_client.c common/tcp_common.c common/tcp_common.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ client/tcp_client.c common/tcp_common.c
tcp_common: common/tcp_common.c common/tcp_common.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ common/tcp_common.c

clean:
	rm -f tcp_server tcp_client
