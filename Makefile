CC=gcc
CFLAGS=-O3 -march=armv8-a+crc+crypto
#sha256:sha256.c sha256-armv8-aarch64.S
#	 $(CC) $(CFLAGS) sha256.c sha256-armv8-aarch64.S -o sha256
sha256:sha256.c sha256_process.c
	 $(CC) $(CFLAGS) sha256.c sha256_process.c -o sha256
clean:
	rm -f *.o sha256
