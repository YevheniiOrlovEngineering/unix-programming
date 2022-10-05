.PHONY: all clean run prepare-env

IDIR 	:= ./include
SRCDIR 	:= ./src
BINDIR 	:= ./bin
LDIR 	:= ./lib

CC 	:= gcc
CFLAGS 	:= -I$(IDIR)
LIBS	:= -lm

WORKDIR ?= test
BINNAME ?= test
_OBJ 	?= test greet
ODIR 	:= $(WORKDIR)/obj
VPATH 	:= $(SRCDIR):$(WORKDIR)

include $(WORKDIR)/params.env

OBJ 	:= $(patsubst %,$(ODIR)/%.o,$(_OBJ))

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
