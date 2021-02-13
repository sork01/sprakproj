#!/bin/bash
#gcc -std=c99 $(pkg-config --cflags glib-2.0) -g -I/usr/include/libxml2 -I/usr/include/glib-2.0 -lxml2 $(pkg-config --libs glib-2.0) sbcsentences.c -o sbcsentences
gcc -std=c99 $(pkg-config --cflags glib-2.0) -O3 -I/usr/include/libxml2 -I/usr/include/glib-2.0 -lxml2 $(pkg-config --libs glib-2.0) sbcsentences.c -o sbcsentences
