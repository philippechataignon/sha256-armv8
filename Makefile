CC=gcc
CFLAGS=-O3 -march=armv8-a+crc+crypto

all: sha256 sha256_asm
sha256_asm:sha256.c sha256-armv8-aarch64.s
	 $(CC) $(CFLAGS) sha256.c sha256-armv8-aarch64.s -o sha256_asm
sha256:sha256.c sha256_process.c
	 $(CC) $(CFLAGS) sha256.c sha256_process.c -o sha256
clean:
	rm -f *.o sha256 sha256_asm
