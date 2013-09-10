CXX = g++
CC = gcc
CFLAGS = -Wall -g
CPPFLAGS = 
LDFLAGS = 

MV := mv -f
RM := rm -f
SED := sed

# install directory
DIR ?= ~/.bin/

# extra include directories
include_dirs :=
CPPFLAGS += $(addprefix -I ,$(include_dirs))
vpath %.h $(include_dirs)

sources := lib.c harness.c
products := harness

harness: lib.o harness.o

objects = $(patsubst %.c,%.o,$(sources))
dependencies = $(patsubst %.c,%.d,$(sources))

# ensure that all is the default target
release:

# include makefiles in subdirectories
# include test/makefile.mk

# test: debug test/test
# 	test/test

.PHONY: clean
clean:
	$(RM) $(objects)
	$(RM) $(products)
	$(RM) $(dependencies)

# include dependencies only if target is not clean
ifneq "$(MAKECMDGOALS)" "clean"
  -include $(dependencies)
endif

.PHONY: debug

debug: CFLAGS += -g
debug: CPPFLAGS += -DDEBUG
debug: $(products)

.PHONY: release

release: CPPFLAGS += -DNDEBUG
release: $(products)

.PHONY: install

install: release
	cp $(products) $(DIR)

# dependency generation
%.d: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -M $< | \
	$(SED) 's,\($*\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp
	$(MV) $@.tmp $@

