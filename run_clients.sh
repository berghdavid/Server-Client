#!/bin/bash

cd src && gcc -pthread -o clients clients.c queue.c && \
./clients && rm clients