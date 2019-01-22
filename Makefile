CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

1cc: $(OBJS)
	$(CC) -o 1cc $(OBJS) $(LDFLAGS)

$(OBJS): 1cc.h

test: 1cc
	./1cc -test
	./test.sh

clean:
	rm -f 1cc *.o *~ tmp*
