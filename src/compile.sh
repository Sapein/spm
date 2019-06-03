#!/bin/sh

OUTPUT="sprocessman"
DEBUG="-g -O0 -Wall -Wextra -pedantic -Werror -Wshadow"
gcc ${DEBUG} main.c ipc.c process/process.c user_processes/user_procs.c manager/manager.c -o "${OUTPUT}"
