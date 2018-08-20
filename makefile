SHELL := /bin/bash

#VARIABLES
CC = gcc
#CrosCC = /home/iasonas/github/openwrt-zsun/staging_dir/toolchain-mips_mips32_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-gcc
RM = rm -f
CFLAGS = -pthread
RUN = server_loc client 

#DIRECTORIES
SRC = src
BIN = bin

#COMMANDS
all: $(RUN)

server_loc: $(SRC)/server.c
	$(CC) $^ -o $(BIN)/$@ $(CFLAGS)

#server: $(SRC)/server.c
#	$(CrosCC) $^ -o $(BIN)/$@ 

#server_thr: $(SRC)/server_th.c
#	$(CrosCC) $^ -o $(BIN)/$@ $(CFLAGS)

client: $(SRC)/client.c
	gcc $^ -o $(BIN)/$@

clean:
	$(RM) $(BIN)/*
