PREFIX := /home/lovro/ARES/buildroot/output/host/bin/riscv32-buildroot-linux-gnu-

CC := $(PREFIX)gcc

CFLAGS := \
	-O2 \
	-Wall \
	-march=rv32imafdc_zicsr_zifencei_zicbom \
	-mabi=ilp32d

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f main