all: libmyhash.so
	gcc test.c -g -o test -lmyhash -L.

libmyhash.so: myhash.c myhash.h
	gcc -Wall -c -g -o libmyhash.so myhash.c -I.

