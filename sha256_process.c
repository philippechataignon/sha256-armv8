#include <stdint.h>
#include <arm_neon.h>

static const uint32_t K[] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

/* from https://fossies.org/linux/nss/nss/lib/freebl/sha256-armv8.c */
#define ROUND(n, a, b, c, d)               \
    {                                      \
        uint32x4_t wt = w0;                \
        uint32x4_t t = vaddq_u32(a, k##n); \
        w0 = vsha256hq_u32(w0, w1, t);     \
        w1 = vsha256h2q_u32(w1, wt, t);    \
        if (n < 12) {                      \
            a = vsha256su0q_u32(a, b);     \
            a = vsha256su1q_u32(a, c, d);  \
        }                                  \
    }

/* Process multiple blocks. The caller is responsible for setting the initial */
/*  state, and the caller is responsible for padding the final block.        */
void sha256_process_arm(uint32_t state[8], const uint8_t data[], uint32_t count)
{
    const uint32x4_t k0 = vld1q_u32(K);
    const uint32x4_t k1 = vld1q_u32(K + 4);
    const uint32x4_t k2 = vld1q_u32(K + 8);
    const uint32x4_t k3 = vld1q_u32(K + 12);
    const uint32x4_t k4 = vld1q_u32(K + 16);
    const uint32x4_t k5 = vld1q_u32(K + 20);
    const uint32x4_t k6 = vld1q_u32(K + 24);
    const uint32x4_t k7 = vld1q_u32(K + 28);
    const uint32x4_t k8 = vld1q_u32(K + 32);
    const uint32x4_t k9 = vld1q_u32(K + 36);
    const uint32x4_t k10 = vld1q_u32(K + 40);
    const uint32x4_t k11 = vld1q_u32(K + 44);
    const uint32x4_t k12 = vld1q_u32(K + 48);
    const uint32x4_t k13 = vld1q_u32(K + 52);
    const uint32x4_t k14 = vld1q_u32(K + 56);
    const uint32x4_t k15 = vld1q_u32(K + 60);

    uint32x4_t h0 = vld1q_u32(state);
    uint32x4_t h1 = vld1q_u32(state + 4);

    while(count--) {

        uint32x4_t a = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data)));
        uint32x4_t b = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 16)));
        uint32x4_t c = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 32)));
        uint32x4_t d = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 48)));

        uint32x4_t w0 = h0;
        uint32x4_t w1 = h1;

        ROUND(0, a, b, c, d)
        ROUND(1, b, c, d, a)
        ROUND(2, c, d, a, b)
        ROUND(3, d, a, b, c)
        ROUND(4, a, b, c, d)
        ROUND(5, b, c, d, a)
        ROUND(6, c, d, a, b)
        ROUND(7, d, a, b, c)
        ROUND(8, a, b, c, d)
        ROUND(9, b, c, d, a)
        ROUND(10, c, d, a, b)
        ROUND(11, d, a, b, c)
        ROUND(12, a, b, c, d)
        ROUND(13, b, c, d, a)
        ROUND(14, c, d, a, b)
        ROUND(15, d, a, b, c)

        h0 = vaddq_u32(h0, w0);
        h1 = vaddq_u32(h1, w1);

        /* next 64 bytes block */
        data += 64;
    }
    vst1q_u32(state, h0);
    vst1q_u32(state + 4, h1);
}
