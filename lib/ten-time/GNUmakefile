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

time.so: _time.c
	$(CC) $(CCFLAGS) -shared -fpic time.c -o time.so

.PHONY: install
install:
	mkdir -p $(TENDIR)/$(LIBRARY)/$(VERSION)
	cp time.so $(TENDIR)/$(LIBRARY)/$(VERSION)

.PHONY: clean
clean:
	rm *.so
