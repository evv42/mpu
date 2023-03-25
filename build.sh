#!/bin/sh
CC=gcc
PRG=mtkview
set -e
set -x
$CC -Wall -O3 -std=gnu99 -funsigned-char -g -c -o apps.o apps.c
$CC -Wall -O3 -std=gnu99 -funsigned-char -g -c -o mpu.o mpu.c
$CC -Wall -O3 -std=gnu99 -funsigned-char -g -c -o xdummy.o xdummy.c
$CC apps.o mpu.o xdummy.o -lm -lX11 -o mpu -lm -lX11 -lXrender
