#!/bin/bash
gcc server.c -o server
echo "SUCCESSFULLY COMPILED SERVER"
gcc clients.c -o clients
echo "SUCCESSFULLY COMPILED CLIENTS"

./server &
sleep 3
./clients