// Random.cpp: implementation of the Random class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4514 ) // unreferenced inline function deleted

#include <stdio.h>
#include <math.h>
#include <float.h>

#include "Random.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const unsigned long Random::RANDOM_MAX = 0xffffffff;

/* A C-program for MT19937: Integer version (1999/10/28)          */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) sets initial values to the working area   */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer.)                     */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software under the Artistic license:       */
/* see the file COPYING distributed together with this code.       */
/* For the verification of the code, its output sequence file      */
/* mt19937int.out is attached (2001/4/2)                           */

/* Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura. */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */


/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A   0x9908b0df /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static void internalSetSeed(unsigned long seed,
														unsigned long *mt, int &mti);
/*
static void internalSetSeed(unsigned long seed_array[N],
														unsigned long *mt, int &mti);
*/

static unsigned long internalGetRandomULong(unsigned long *mt, int &mti);


// Static variables for quickRand functions
static unsigned long mt[N]; /* The array for the state vector  */
static int mti = N+1;       /* mti == N+1 means mt[N] is not initialized */



Random::Random(unsigned long seed)
{
	m_mt = new unsigned long [N];
	setSeed(seed);
}

Random::~Random()
{
	if (m_mt != NULL) delete [] m_mt;
}

void Random::setQuickRandSeed(unsigned long seed)
{
	internalSetSeed(seed, mt, mti);
}

unsigned long Random::quickRand()
{
	if (mti == N+1)   /* if sgenrand() has not been called, */
		setQuickRandSeed(4357); /* a default initial seed is used   */
	
  return internalGetRandomULong(mt, mti); 
}




void Random::setSeed(unsigned long seed)
{
	internalSetSeed(seed, m_mt, m_mti);
}

unsigned long Random::getRandomULong()
{
	return internalGetRandomULong(m_mt, m_mti); 
}



/*inline*/ double Random::dRandNormDist(double mean, double stddev)
{
	const double two_pi = 6.28318530717958623200;
	
	double r = -log( dRand() );
	
	while( !_finite(r) )
	{
		r = -log( dRand() );
	}
	
	return mean + stddev * r * cos( two_pi * dRand() );
}



/* Initializing the array with a seed */
//void 
//sgenrand(seed)
//    unsigned long seed;	
void internalSetSeed(unsigned long seed,
										 unsigned long mt[N], int &mti)
{
	int i;
	
	for( i = 0; i < N; i++) {
		mt[i] = seed & 0xffff0000;
		seed = 69069 * seed + 1;

		mt[i] |= (seed & 0xffff0000) >> 16;
		seed = 69069 * seed + 1;
	}

	mti = N;
}

/* Initialization by "sgenrand()" is an example. Theoretically,      */
/* there are 2^19937-1 possible states as an intial state.           */
/* This function allows to choose any of 2^19937-1 ones.             */
/* Essential bits in "seed_array[]" is following 19937 bits:         */
/*  (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]. */
/* (seed_array[0]&LOWER_MASK) is discarded.                          */ 
/* Theoretically,                                                    */
/*  (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]  */
/* can take any values except all zeros.                             */
//void
//lsgenrand(seed_array)
//    unsigned long seed_array[]; 
/* the length of seed_array[] must be at least N */
/*
void internalSetSeed(unsigned long seed_array[N],
										 unsigned long mt[N], int &mti)
{
	int i;
	
	for( i = 0; i < N; i++) 
		mt[i] = seed_array[i];

	mti = N;
}
*/

//unsigned long 
//genrand()
unsigned long internalGetRandomULong(unsigned long mt[N], int &mti)
{
	unsigned long y;
	static unsigned long mag01[2] = {0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */
	
	if (mti >= N) { /* generate N words at one time */
		int kk;
		
		for (kk=0;kk<N-M;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (;kk<N-1;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
		
		mti = 0;
	}
  
	y = mt[mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);
	
	return y; 
}

#define TEST_RND( NSAMPLES, RNDFUNC, AVG, AVG2 ) \
{ \
	double x, sum = 0, sum2 = 0, min = 1000000, max = -1000000; \
	for(long ii = 0; ii < NSAMPLES; ii++) \
	{ \
		x=RNDFUNC; if(x<min)min=x; if(x>max)max=x; sum+=x; sum2+=x*x; \
	} \
	printf("%s - %d samples:\n", #RNDFUNC, NSAMPLES); \
	printf("  average: %g (expected %g)\n", sum / NSAMPLES, (double)AVG); \
	printf("  average squares: %g (expected %g)\n", sum2 / NSAMPLES, (double)AVG2); \
  printf("  min: %g, max: %g\n", min, max); \
}

void Random::printTestResult(int nSamples)
{
	TEST_RND(nSamples, dRandNormDist(), 0, 1);
	TEST_RND(nSamples, fRandNormDist(), 0, 1);
	TEST_RND(nSamples, dRand(), 0.5, 1.0/3);
	TEST_RND(nSamples, fRand(), 0.5, 1.0/3);
	TEST_RND(nSamples, iRand(), 0.5, 0.5);
}