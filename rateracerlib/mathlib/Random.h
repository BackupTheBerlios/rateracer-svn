#pragma once

// seed for random numbers
#define nseed init_genrand

// random float [0, 1]
#define nrand genrand_float1

// random float [-1, 1]
#define snrand genrand_signed_float1

// prototypes for Mersienne Twister pseudoranom generator functions
// from the file mt19937ar-cok.c
extern "C" {
  void init_genrand(unsigned long s);
  unsigned long genrand_int32(void);
  float genrand_float1(void);
  float genrand_signed_float1(void);
}
