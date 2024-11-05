CFLAGS=--std=c17 -Wall -pedantic -Isrc/ -ggdb -Wextra -Werror -DDEBUG

CC=clang

# export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/mopstream/Documents/ITMO-4-course/distributed/pa4"

LD_PRELOAD=/home/mopstream/Documents/ITMO-4-course/distributed/pa3/libruntime.so ./pa4 -p 2 10 20

main:
	$(CC) -L. -lruntime -std=c99 -Wall -pedantic *.c -o pa4

debug:
	$(CC) -L. -lruntime -std=c99 -Wall -pedantic *.c -g -o jopa


tar:
	tar -zcvf pa4.tar.gz .