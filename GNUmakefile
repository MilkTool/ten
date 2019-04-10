IDIRS   := -I ten-lang/src/
LDIRS   := -L ten-lang/
CCFLAGS := -Wall -lreadline -ldl
PROFILE ?= release
PREFIX  ?= /usr/local/
BINDIR  ?= $(PREFIX)/bin

ifeq ($(PROFILE),release)
    CCFLAGS += -O2 -D NDEBUG -l ten
    POSTFIX := 
else
    ifeq ($(PROFILE),debug)
        CCFLAGS += -g -O0 -l ten-debug
        POSTFIX := -debug
    else
        $(error "Invalid build profile")
    endif
endif

ten$(POSTFIX): cli.c ten-load/ten_load.h ten-load/ten_load.c ten-lang/libten.so
	$(CC) $(CCFLAGS) $(IDIRS) $(LDIRS) ten-load/ten_load.c cli.c -o ten$(POSTFIX)

ten-lang/libten.so: ten-lang/
	PROFILE=$(PROFILE) $(MAKE) -C ten-lang/

.PHONY: install
install:
	PREFIX=$(PREFIX) PROFILE=$(PROFILE) $(MAKE) -C ten-lang/ install
	mkdir  -p $(BINDIR)
	cp ten$(POSTFIX) $(BINDIR)

.PHONY: clean
clean:
	- PROFILE=$(PROFILE) $(MAKE) -C ten-lang/ clean
	- rm ten ten$(POSTFIX)
	- rm ten-load/*.o
