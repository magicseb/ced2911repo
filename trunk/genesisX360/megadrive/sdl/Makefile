# Genesis Plus - Sega Mega Drive emulator
# (c) 1999, 2000, 2001, 2002, 2003  Charles MacDonald
# Linux/SDL Port (c) 2004 Stuart Dalton

# -DLSB_FIRST - Leave undefined for big-endian processors.
# -DDEBUG     - Enable debugging code
# -DX86_ASM   - Enable inline x86 assembly code in Z80 emulator
# -DALIGN_LONG - Align longword reads and writes to memory

CC = gcc
CFLAGS = -I. -I.. -I../cpu -I../c68k -I../sound -I ../cz80 `sdl-config --cflags` -Wall -Os -march=athlon-xp
FLAGS = $(CFLAGS) -DLSB_FIRST -DCPU68K_USE_C68K -DCPUZ80_USE_CZ80
LIBS = -lz -lm `sdl-config --libs`

TARGET = genplus

OBJS = genesis.o vdp.o render.o system.o unzip.o fileio.o loadrom.o io.o mem68k.o memz80.o membnk.o memvdp.o cpu68k.o
OBJS += sound.o fm.o sn76496.o
OBJS += error.o main.o
PROBJS = cz80.o
PROBJS += c68k.o c68kexec.o
GSRC = ../c68k/c68k_ini.inc ../c68k/c68k_op0.inc ../c68k/c68k_op1.inc ../c68k/c68k_op2.inc ../c68k/c68k_op3.inc
GSRC += ../c68k/c68k_op4.inc ../c68k/c68k_op5.inc ../c68k/c68k_op6.inc ../c68k/c68k_op7.inc ../c68k/c68k_op8.inc
GSRC += ../c68k/c68k_op9.inc ../c68k/c68k_opA.inc ../c68k/c68k_opB.inc ../c68k/c68k_opC.inc ../c68k/c68k_opD.inc
GSRC += ../c68k/c68k_opE.inc ../c68k/c68k_opF.inc

all: rm-exe $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rclean:
	rm -f $(OBJS) $(PROBJS) $(TARGET) $(GSRC) ../c68k/gen68k

rm-exe:
	-rm -f $(TARGET)

$(TARGET): $(OBJS) $(PROBJS)
	$(CC) -o $(TARGET) $(OBJS) $(PROBJS) $(LIBS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $@ $(FLAGS)

%.o: ../%.c ../%.h
	$(CC) -c $< -o $@ $(FLAGS)

%.o: ../cpu/%.c ../cpu/%.h
	$(CC) -c $< -o $@ $(FLAGS)

%.o: ../cpu/%.c
	$(CC) -c $< -o $@ $(FLAGS)

%.o: ../cz80/%.c $(GSRC)
	$(CC) -c $< -o $@ $(FLAGS)

%.o: ../c68k/%.c $(GSRC)
	$(CC) -c $< -o $@ $(FLAGS)

$(GSRC): ../c68k/gen68k
	cd ../c68k && ./gen68k

../c68k/gen68k: ../c68k/gen68k.c
	gcc -DC68K_GEN -I .. ../c68k/gen68k.c -o ../c68k/gen68k

%.o: ../sound/%.c ../sound/%.h
	$(CC) -c $< -o $@ $(FLAGS)

dist: $(TARGET)
	strip $(TARGET)
	upx --best $(TARGET)
