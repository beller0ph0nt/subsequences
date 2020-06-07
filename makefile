CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

INCLUDE_DIR = -I$(abspath .)

CFLAGS = -g -Wall -DDEBUG

CLI_OBJ = client.o client-library.o common-library.o
CLI_BIN = client

SRV_OBJ = server.o server-library.o common-library.o
SRV_LIB = -lpthread
SRV_BIN = server

.PHONY: all clean

all: $(CLI_BIN) $(SRV_BIN)

$(CLI_BIN): $(CLI_OBJ)
	$(CC) -o $@ $^ $(CLI_LIB) $(LIBPATH)

$(SRV_BIN): $(SRV_OBJ)
	$(CC) -o $@ $^ $(SRV_LIB) $(LIBPATH)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE_DIR) -o $@ $^

clean:
	rm -f $(CLI_OBJ) $(CLI_BIN) $(SRV_OBJ) $(SRV_BIN)
