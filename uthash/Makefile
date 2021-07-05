#CC=clang
HASHDIR = src
#FUNCS = BER SAX FNV OAT JEN SFH 
#SPECIAL_FUNCS = MUR
#UTILS = emit_keys
PROGS = test1
CFLAGS = -I$(HASHDIR) 
CFLAGS += -g
CFLAGS += -Wall -lpthread
CFLAGS += ${EXTRA_CFLAGS}




all: $(PROGS) $(UTILS) $(PLAT_UTILS) $(FUNCS) $(SPECIAL_FUNCS) $(TEST_TARGET) 



clean:	
	rm -f $(UTILS) $(PLAT_UTILS) $(PROGS) test*.out keystat.??? example *.exe
	rm -rf *.dSYM
