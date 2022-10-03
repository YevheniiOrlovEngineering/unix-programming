LINK_TARGET = lab1
OBJS = \
	main.o \
	file.o \
	log.o


CC = gcc

REBUILDABLES = ${OBJS} ${LINK_TARGET}


%.o: %.c 
	${CC} -c -o $@ $<

clean : 
  	rm -f ${REBUILDABLES}
  	echo Clean done


all: ${LINK_TARGET} run


${LINK_TARGET}: ${OBJS}
  ${CC} -o $@ $^


run: 
	./${LINK_TARGET}

log:
	gcc
