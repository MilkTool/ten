IDIRS   := -I ten-lang/
LDIRS   := -L ten-lang/
CCFLAGS := -Wall -lreadline -ldl -g
PROFILE ?= release

ifeq ($(PROFILE),release)
    CCFLAGS += -O3 -D NDEBUG -l ten
else
    ifeq ($(PROFILE),debug)
        CCFLAGS += -g -O0 -l ten-debug
    else
        $(error "Invalid build profile")
    endif
endif

ten: ten.c ten-load/ten_load.h ten-load/ten_load.c ten-lang/libten.so
	$(CC) $(CCFLAGS) $(IDIRS) $(LDIRS) ten-load/ten_load.c ten.c -o ten

ten-lang/libten.so: ten-lang/
	$(MAKE) -C ten-lang/

