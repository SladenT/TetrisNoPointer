/*******************************************************************************************
*	A basic implementation of xoroshiro128++, seeded using SplitMix64.  Should be fast and random.
*	
*   Created by Davis Teigeler
********************************************************************************************/
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "rng.h"

static inline uLong rotl(const uLong x, int k) {
	return (x << k) | (x >> (64 - k));
}

static uLong s[2];
static uLong smSeed;

static uLong SplitMix64()
{
	uLong z = (smSeed += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

static uLong next(void) {
	const uLong s0 = s[0];
	uLong s1 = s[1];
	const uLong result = rotl(s0 + s1, 17) + s0;

	s1 ^= s0;
	s[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
	s[1] = rotl(s1, 28); // 

	return result;
}

void SetSeed(uLong L)
{
	smSeed = L;
	s[0] = SplitMix64();
	s[1] = SplitMix64();
	//call next twice so we don't just return the seed added together.
	next();
	next();
}

void SetSeedWithClock(void)
{
	SetSeed(time(NULL));
}

uInt GetNextInt(uInt max)
{
	uInt res, range, maxRoll, rngMax;
	rngMax = 0xFFFFFFFF;
	range = rngMax/max;
	maxRoll = max * range;
	
	do {res = (uInt)(next());}
	while (res > maxRoll);
	
	return (res % max);
}