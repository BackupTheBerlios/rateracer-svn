#pragma once

#include <limits.h>

class Random  
{
public:
	Random(unsigned long seed = 4357); // default initial seed
	virtual ~Random();

	static const unsigned long RANDOM_MAX;
	
	// Static functions for global random generator

	static void setQuickRandSeed(unsigned long seed);
	static unsigned long quickRand();

	// Functions for object with local random generator

	void setSeed(unsigned long seed);
	unsigned long getRandomULong();


	// [lower, upper] random float
	double dRand(double lower, double upper);
	double dRand(double upper /* lower = 0 */);
	double dRand(/*lower = 0, upper = 1*/);

	// Normal distributed random float N(mean, stddev)
	double dRandNormDist(double mean = 0, double stddev = 1);


	// [lower, upper] random float
	float fRand(float lower, float upper);
	float fRand(float upper /* lower = 0 */);
	float fRand(/*lower = 0, upper = 1*/);


	// Normal distributed random float N(mean, stddev)
	float fRandNormDist(float mean = 0, float stddev = 1);

	// [lower, upper] random integer
	int iRand(int lower, int upper);
	int iRand(int upper /* lower = 0 */);
	int iRand(/*lower = 0, upper = INT_MAX*/);

	
	// Routine for verifying correctness...
	void printTestResult(int nSamples = 10000);

private:
	// Internal variables for generating random numbers
	unsigned long *m_mt;
	int m_mti;
};

inline double Random::dRand(double lower, double upper)
{
	return lower + (upper - lower) * (double(getRandomULong()) / RANDOM_MAX);
}
inline double Random::dRand(double upper)
{
	return dRand(0, upper);
}
inline double Random::dRand()
{
	return dRand(0, 1);
}

inline float Random::fRand(float lower, float upper)
{
	return (float) dRand(lower, upper);
}
inline float Random::fRand(float upper)
{
	return (float) dRand(0, upper);
}
inline float Random::fRand()
{
	return (float) dRand(0, 1);
}


inline float Random::fRandNormDist(float mean, float stddev)
{
	return (float) dRandNormDist(mean, stddev);
}

inline int Random::iRand(int lower, int upper)
{
	int r = lower + (int)dRand((double)upper - (double)lower + 1);
	if (r > upper) r = upper;
	return r;
}
inline int Random::iRand(int upper)
{
	return iRand(0, upper);
}
inline int Random::iRand()
{
	return iRand(0, INT_MAX);
}


// These global functions are kept for backwards compatibility and familiarity:

// random float [0, 1]
inline float nrand()
{
	return (float(Random::quickRand()) / Random::RANDOM_MAX);
}

// random float [-1, 1] 
inline float snrand()
{
	return (-1.0f + 2.0f * ( float(Random::quickRand()) / Random::RANDOM_MAX ));
}

// seed for Random
inline void nseed(unsigned long l)
{
	Random::setQuickRandSeed(l);
}
