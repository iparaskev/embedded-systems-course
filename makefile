SHELL := /bin/bash

#VARIABLES
CC = gcc
CrosCC = /home/iasonas/github/openwrt-zsun/staging_dir/toolchain-mips_mips32_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc
RM = rm -f
CFLAGS = -pthread
RUN = server_loc client server
DEPS = constants.h data_structures.h

#DIRECTORIES
SRC = src
BIN = bin
HEADERS = headers
OBJD = obj

#COMMANDS
all: $(RUN)

server_loc: $(SRC)/server.c $(SRC)/data_structures.c
	$(CC) $^ -o $(BIN)/$@ $(CFLAGS) -I $(HEADERS)

server: $(SRC)/server.c $(SRC)/data_structures.c
	$(CrosCC) $^ -o $(BIN)/$@ $(CFLAGS) -I $(HEADERS)

#server_thr: $(SRC)/server_th.c
#	$(CrosCC) $^ -o $(BIN)/$@ $(CFLAGS)

client: $(SRC)/client.c
	gcc $^ -o $(BIN)/$@ -I $(HEADERS)

clean:
	$(RM) $(BIN)/*



