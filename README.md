# SHA-256

This is a basic implementation of a SHA-256 hash according to the [FIPS
180-4 standard](http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf)
in C. The code is not optimized but corresponds line by line to the standard.

The algorithm to process a small block of data (256 bits = 32 bytes)
is quite simple and very well described in the standard. This fork
implements that multiple blocks are processing in one call. Implementing 
the padding for the last black is the tricky part.

No dependencies except for the gnu libc standard library for bswap_64 function.
It can be compiled with `make`.

Usage:
```
./main <input file>
```
