CCFLAGS := -Wall
PROFILE ?= release
PREFIX  ?= /usr/local/
BINDIR  ?= $(PREFIX)/bin

ifeq ($(PROFILE),release)
    CCFLAGS += -O2 -D NDEBUG
    LIBS    := -l ten -l tml
    POSTFIX := 
else
    ifeq ($(PROFILE),debug)
        CCFLAGS += -g -O0
        LIBS    := -l ten-debug
        POSTFIX := -debug
    else
        $(error "Invalid build profile")
    endif
endif

LIBS += -l readline -l dl -l m -l tml

ten$(POSTFIX): ten.c
	$(CC) $(CCFLAGS) $(IDIRS) $(LDIRS) ten.c $(LIBS) -o ten$(POSTFIX)

.PHONY: install
install:
	mkdir  -p $(BINDIR)
	cp ten$(POSTFIX) $(BINDIR)

.PHONY: clean
clean:
	- rm ten$(POSTFIX)

