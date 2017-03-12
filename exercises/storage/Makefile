.PHONY: all

all:
	gcc -o read read.c
	gcc -o generate generate.c
	gcc -o write write.c
	gcc -o write_no_o_dsync write_no_o_dsync.c
	gcc -o mread mread.c

read:
	gcc -o read read.c

generate:
	gcc -o generate generate.c

write:
	gcc -o write write.c

mread:
	gcc -o mread mread.c

debug:
	gcc -o read read.c -ggdb
