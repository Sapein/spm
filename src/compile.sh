#!/bin/sh

OUTPUT="sprocessman"
gcc main.c ipc.c process/process.c user_processes/user_procs.c manager/manager.c -o "${OUTPUT}"
