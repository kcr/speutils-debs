CC=gcc


BIN1=example1
BIN2=example2

#replace spu-elf-gcc to spu-gcc if you do not have it
SPU_CC=spu-elf-gcc

SPU_BIN1 = spu_example1
SPU_BIN2 = spu_example2

SPU_EMBED = embedspu

LDADD1=${SPU_BIN1}.eo.o -lspeutils
LDADD2=${SPU_BIN2}.eo.o -lspeutils

all: ppu

spu:
	$(SPU_CC) ${SPU_BIN1}.c -o ${SPU_BIN1}
	$(SPU_CC) ${SPU_BIN2}.c -o ${SPU_BIN2}

embed: spu
	$(SPU_EMBED) ${SPU_BIN1}_handle ${SPU_BIN1} ${SPU_BIN1}.eo.o
	$(SPU_EMBED) ${SPU_BIN2}_handle ${SPU_BIN2} ${SPU_BIN2}.eo.o

ppu: spu embed
	$(CC) ${BIN1}.c -o ${BIN1} ${LDADD1}
	$(CC) ${BIN2}.c -o ${BIN2} ${LDADD2}
