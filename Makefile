.PHONY: all clean run prepare-env build

IDIR 		:= ./include
SRCDIR 		:= ./src
BINDIR 		:= ./bin
LDIR 		:= ./lib

CC 		:= gcc
CFLAGS 		:= -I$(IDIR)
LIBS		:= -lm

WORKDIR 	?= test
BINNAME 	?= test
_OBJ 		?= test greet
VPATH 		:= $(SRCDIR):$(WORKDIR)

CONFIG_FILE 	:= config.mk
CONFIG  	:= $(WORKDIR)/$(CONFIG_FILE)

ODIR 		:= $(WORKDIR)/obj
OBJ 		= $(patsubst %,$(ODIR)/%.o,$(_OBJ))

include $(CONFIG)

all: prepare-env build run

build: prepare-env $(BINDIR)/$(BINNAME)

prepare-env:
	mkdir -p $(BINDIR) $(ODIR)

run:
	$(BINDIR)/$(BINNAME)

clean:
	rm -rf $(ODIR) $(BINDIR)/$(BINNAME)

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/$(BINNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

#TODO: override mk variables from $WORKDIR/config

#--lab-1-part-1:
#	$(eval WORKDIR := lab-1/part-1)
#--lab-1-part-2:
#	$(eval WORKDIR := lab-1/part-2)
#
#--get-config:
#	$(eval BINNAME := $(shell sed -n 1p $(WORKDIR)/.env))
#	$(eval _OBJ    := $(shell sed -n 2p $(WORKDIR)/.env))

#
#lab-1-1: --lab-1-part-1 --get-config all
#
#lab-1-2: --lab-1-part-2 all
