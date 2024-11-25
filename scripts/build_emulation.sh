#!/bin/bash
aarch64-linux-gnu-g++ -static \
    src/main.cpp \
    -o build/kiosk.elf \
    -I/usr/aarch64-linux-gnu/include \
    -L/usr/aarch64-linux-gnu/lib \
    -lEGL -lGLESv2 -lbcm_host -lgpiod \
    -std=c++17

chmod +x build/kiosk.elf