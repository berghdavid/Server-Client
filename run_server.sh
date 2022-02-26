#!/bin/bash

cd src && gcc -pthread -o server server.c queue.c && \
./server && rm server