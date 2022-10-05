.PHONY: all clean run prepare-env

include $(WORKDIR)/params.env

IDIR 	:= ./include
SRCDIR 	:= ./src
BINDIR 	:= ./bin
LDIR 	:= ./lib

CC 	:= gcc
CFLAGS 	:= -I$(IDIR)
LIBS	:= -lm

WORKDIR ?= example
BINNAME ?= help
_OBJ 	?= help.o
ODIR 	:= $(WORKDIR)/obj
OBJ 	:= $(patsubst %,$(ODIR)/%,$(_OBJ))

VPATH 	:= $(SRCDIR):$(WORKDIR)


all: prepare-env build run

prepare-env:
	mkdir -p $(BINDIR) $(ODIR)

build: $(BINDIR)/$(BINNAME)

run:
	$(BINDIR)/$(BINNAME)

clean:
	rm -rf $(ODIR) $(BINDIR)/$(BINNAME)

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/$(BINNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
