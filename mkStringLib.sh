#!/usr/bin/env bash

arm-none-eabi-gcc -c string.c
ar -r strlib string.o