#!/usr/bin/env bash

qemu-system-arm -M versatilepb -m 128M -kernel LAB4.bin -serial mon:stdio