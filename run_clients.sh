#!/bin/bash

cd src && gcc -pthread -o clients clients.c && \
./clients && rm clients