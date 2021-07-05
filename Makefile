# For multiple programs using a single source file each,
# we can just define 'progs' and create custom targets.
PROGS   = mydba_sys
#PROGS += pingd
MORE_PROGS = kern_test

CLEANFILES = $(PROGS) *.o
NO_MAN=
CFLAGS = -O2 -pipe
CFLAGS += -Wall -Wunused-function
CFLAGS += -I /root/waf2/sys # -I/home/luigi/FreeBSD/head/sys -I../sys
CFLAGS += -Wextra


LDFLAGS += -lpthread -lpcre -lm
LDFLAGS += -lrt -lm # needed on linux, does not harm on BSD
#SRCS = pkt-gen.c

all: $(PROGS)


mydba_sys: mydba_sys.o
	$(CC) $(CFLAGS) -o mydba_sys mydba_sys.o -lpcre -lpthread -lpcap -lm


clean:
	-@rm -rf $(CLEANFILES)

