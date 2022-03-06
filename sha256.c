#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <byteswap.h>

void sha256_block_data_order (uint32_t *ctx, const void *in, size_t num);

// SHA-256 initial hash value
const uint32_t H_0[8] = {
	0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19,
};

uint8_t buffer[1<<22]; // 4M

int main(int argc, char* argv[]) {
	uint32_t i;

	// check arguments
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return -1;
	}

	// open input file
	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		fprintf(stderr, "Error opening file '%s' for reading.\n", argv[1]);
		return -1;
	}

	// initialize hash value
	uint32_t H[8];
	memcpy(H, H_0, sizeof(H_0));

	// read file and calculate hash
	uint64_t total = 0;
	size_t len;
	while (len = fread(buffer, 1, sizeof(buffer), fp)) {
        if (ferror(fp)) {
		    fprintf(stderr, "Error reading file '%s'.\n", argv[1]);
            return 1;
        }
		total += len;
		// preserve value of len by breaking on end of file
		if (feof(fp)) {
			break;
		}
		sha256_block_data_order(H, buffer, len / 64);
	}
	uint32_t n = len / 64;              // # full blocks
    if (n > 0) {
	    sha256_block_data_order(H, buffer, n);
    }
	uint32_t base = 64 * n;             // addr base last block
    len = len % 64;                     // read on last block
	buffer[base + len] = 0x80;
	// add padding
	if (len < 56) {
		// padd current block to 56 byte
		i = len + 1;
	} else {
		// fill up current block and update hash
		for (i = len + 1; i < 64; i++) {
			buffer[base + i] = 0x00;
		}
		sha256_block_data_order(H, buffer + base, 1);
		// add (almost) one block of zero bytes
		i = 0;
	}
	for (; i < 56; i++) {
		buffer[base + i] = 0x00;
	}
	// add message length in bits in big endian
	*(uint64_t*)(buffer + base + 56) = bswap_64(total << 3);
	sha256_block_data_order(H, buffer + base, 1);

	for (i = 0; i < 8; i++) {
		printf("%08x", H[i]);
	}
	printf("  %s\n", argv[1]);
	return 0;
}
