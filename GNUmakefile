IDIRS   := -I ten-lang/
LDIRS   := -L ten-lang/
CCFLAGS := -Wall -lreadline
PROFILE ?= release

ifeq ($(PROFILE),release)
    CCFLAGS += -O3 -D NDEBUG -l ten
else
    ifeq ($(PROFILE),debug)
        CCFLAGS += -G -O0 -l ten-debug
    else
        $(error "Invalid build profile")
    endif
endif

build: ten.c ten-lang/
	$(CC) $(CCFLAGS) $(IDIRS) $(LDIRS) ten.c

ten-lang/libten.so: ten-lang/
	$(MAKE) -C ten-lang/

