PROFILE ?= release
CCFLAGS := -Wall -Wno-unused -Wno-multichar
LINK    := -lm
CC      ?= gcc
PREFIX  ?= /usr/local/
TENDIR  ?= $(PREFIX)/lib/ten

VERSION := $(shell cat VERSION | tr . -)
LIBRARY := $(shell cat LIBRARY)

ifeq ($(PROFILE),release)
    CCFLAGS += -O3 -D NDEBUG -l ten
else
    ifeq ($(PROFILE),debug)
        CCFLAGS += -g -O0 -l ten-debug
    else
        $(error "Invalid build profile")
    endif
endif

fs.so: fs.c
	$(CC) $(CCFLAGS) -shared -fpic fs.c -o fs.so

.PHONY: install
install:
	mkdir -p $(TENDIR)/$(LIBRARY)/$(VERSION)
	cp fs.so $(TENDIR)/$(LIBRARY)/$(VERSION)

.PHONY: clean
clean:
	rm *.so
