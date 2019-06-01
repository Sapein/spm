#!/bin/sh

gcc -O0 -g main.c ipc.c process/process.c process/user_procs.c manager/manager.c
