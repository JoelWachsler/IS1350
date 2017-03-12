CC=gcc
DEBUG=-g

.PHONY: all

all:
	$(CC) -o freq freq.c
	$(CC) -o freq_doubles freq_doubles.c

debug:
	$(CC) -o freq freq.c $(DEBUG)
	$(CC) -o freq_doubles freq_doubles.c $(DEBUG)
