#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <byteswap.h>

void sha256_process_arm(uint32_t state[8], const uint8_t data[], uint32_t length);

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

uint8_t buffer[1<<24]; // 16M

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
        return -2;
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
            return -3;
        }
        total += len;
        // preserve value of len by breaking on end of file
        if (feof(fp)) {
            break;
        }
        if (len % 64 != 0) {
            fprintf(stderr, "Error: len must be a 64 multiple.\n");
            return -4;
        }
        sha256_process_arm(H, buffer, len / 64);
    }
    uint32_t n = len / 64;              // # of full blocks
    if (n > 0) {
        sha256_process_arm(H, buffer, n);
    }
    uint8_t* base = buffer + 64 * n;    // addr base of last block
    len = len - 64 * n;                 // # of bytes in last block
    base[len++] = 0x80;                 // add end bit/byte

    if (len <= 56) {
        // padd current block to 56 byte
        memset(base + len, 0, 56 - len);
    } else {
        // fill up current block to 64 byte and update hash
        memset(base + len, 0, 64 - len);
        // extra process
        sha256_process_arm(H, base, 1);
        // add (almost) one block of zero bytes
        memset(base, 0, 56);
    }
    // add message length in bits in big endian
    *(uint64_t*)(base + 56) = bswap_64(total << 3);
    sha256_process_arm(H, base, 1);

    for (i = 0; i < 8; i++) {
        printf("%08x", H[i]);
    }
    printf("  %s\n", argv[1]);
    return 0;
}
