#!/bin/bash
gcc -pthread -o server server.c & echo "SUCCESSFULLY COMPILED SERVER"
gcc clients.c -o clients & echo "SUCCESSFULLY COMPILED CLIENTS"

xterm -hold -e ./server & sleep 4s
xterm -hold -e ./clients