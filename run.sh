#!/bin/bash
gcc server.c -o server
echo "SUCCESSFULLY COMPILED SERVER"
gcc clients.c -o clients
echo "SUCCESSFULLY COMPILED CLIENTS"

xterm -hold -e ./server &
sleep 3
xterm -hold -e ./clients