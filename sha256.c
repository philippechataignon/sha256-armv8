#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
	uint64_t bits = 0;
	uint8_t buffer[64];
	size_t len;
	while (len = fread(buffer, 1, sizeof(buffer), fp)) {
        if (ferror(fp)) {
		    fprintf(stderr, "Error reading file '%s'.\n", argv[1]);
            return 1;
        }
		bits += len * 8;
		// preserve value of len by breaking on end of file
		if (feof(fp)) {
			break;
		}
		sha256_block_data_order(H, buffer, 1);
	}

	buffer[len] = 0x80;
	// add padding
	if (len < 56) {
		// padd current block to 56 byte
		i = len + 1;
	} else {
		// fill up current block and update hash
		for (i = len + 1; i < 64; i++) {
			buffer[i] = 0x00;
		}
		sha256_block_data_order(H, buffer,1);
		// add (almost) one block of zero bytes
		i = 0;
	}
	while (i < 56) {
		buffer[i++] = 0x00;
	}
	// add message length in bits in big endian
	for (i = 0; i < 8; i++) {
		buffer[63 - i] = bits >> (i * 8);
	}
	sha256_block_data_order(H, buffer,1);

	// convert hash to char array (in correct order)
	for (i = 0; i < 8; i++) {
		buffer[4 * i + 0] = H[i] >> 24;
		buffer[4 * i + 1] = H[i] >> 16;
		buffer[4 * i + 2] = H[i] >>  8;
		buffer[4 * i + 3] = H[i];
	}

	// print hash
	printf("Hash:\t");
	for (i = 0; i < 32; i++) {
		printf("%02x", buffer[i]);
	}
	printf("\n");
	return 0;
}
