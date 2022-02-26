#!/bin/bash

cd src && gcc -pthread -o server server.c && \
./server && rm server