#!/bin/sh

OUTPUT="sprocessman"
DEBUG="-g -D_POSIX_C_SOURCE=200809 -std=c11 -O0 -Wall -Wextra -pedantic -Werror -Wshadow -fdiagnostics-color=always"
cc ${DEBUG} main.c ipc.c process/process.c user_processes/user_procs.c manager/manager.c logging.c -o "${OUTPUT}"
