/* ==========================================================================

nc_mtrandom.c

Mersenne Twister random number generator
Based on code by Makoto Matsumoto, Takuji Nishimura

Reference
M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

========================================================================== */

#include "mtrand.h"
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>


#define MAGIC_MASK_A          0x9908b0df
#define MAGIC_MASK_B          0x9d2c5680
#define MAGIC_MASK_C          0xefc60000
#define MUB_MASK              0x80000000
#define MLB_MASK              0x00000001
#define NUB_MASK              0x7fffffff
#define STATES                624
#define PERIOD_STATE          397
#define MAGIC_RSHIFT_NUMBER_U 11
#define MAGIC_LSHIFT_NUMBER_S 7
#define MAGIC_LSHIFT_NUMBER_T 15
#define MAGIC_RSHIFT_NUMBER_L 18

static u32  m_auState[STATES];
static u32 const* m_puEndState = m_auState + STATES;
static u32 const* m_puNextState = NULL;
static u32  m_uShiftCounter8;
static u32  m_uCurrentNumber8;
static u32  m_uShiftCounter16;
static u32  m_uCurrentNumber16;

void FASTCALL FncMtSrand(u32 uSeed) {
	u32* puPrevState = m_auState;
	u32* puState = puPrevState + 1;
	u32  uOrder = 1;

	*puPrevState = uSeed;

	while (puState < m_puEndState) {
		*puState = (1812433253 * (*puPrevState ^ (*puPrevState >> 30)) + uOrder);
		puState++;
		puPrevState++;
		uOrder++;
	}

	m_puNextState = m_puEndState;
}

void FASTCALL FncMtSrands(u32 const* auSeed, u32 uSeeds) {

	u32* puPrevState = m_auState;
	u32* puState = puPrevState + 1;
	u32  uRankOfState = 1;
	u32  uRankOfSeed = 0;
	u32  uIterate = 0;
	u32 const* puSeed = auSeed;
	u32 const* puEndSeed = auSeed + uSeeds;

	FncMtSrand(19650218);


	uIterate = (STATES > uSeeds) ? STATES : uSeeds;

	while (uIterate) {

		*puState = (*puState ^ ((*puPrevState ^ (*puPrevState >> 30)) * 1664525))
			+ *puSeed + uRankOfSeed;

		puState++;
		puPrevState++;
		uRankOfState++;
		puSeed++;
		uRankOfSeed++;
		uIterate--;

		if (puState == m_puEndState) {
			*m_auState = *(m_auState + STATES - 1);
			uRankOfState = 1;
			puPrevState = m_auState;
			puState = puPrevState + 1;
		}

		if (puSeed == puEndSeed) {
			uRankOfSeed = 0;
			puSeed = auSeed;
		}
	}


	uIterate = STATES - 1;

	while (uIterate) {

		*puState = (*puState ^ ((*puPrevState ^ (*puPrevState >> 30)) * 1566083941))
			- uRankOfState;

		puState++;
		puPrevState++;
		uRankOfState++;
		uIterate--;

		if (puState == m_puEndState) {
			*m_auState = *(m_auState + STATES - 1);
			uRankOfState = 1;
			puPrevState = m_auState;
			puState = puPrevState + 1;
		}

	}

	*m_auState = 0x80000000;
	m_puNextState = m_puEndState;
}

void FASTCALL FncMtRandomize(void) {
	FncMtSrand(time(NULL));
}

u32 FASTCALL FncMtRand(void) {

	u32 uState;

	if (m_puNextState == NULL)
		FncMtSrand(19650218);

	if (m_puNextState == m_puEndState) {

		u32* puState = m_auState;
		u32 const* puNextState = puState + 1;
		u32 const* puPointState;
		u32 const* puEndState;

		puEndState = puState + (STATES - PERIOD_STATE);
		puPointState = puState + PERIOD_STATE;

		while (puState < puEndState) {

			*puState =
				*puPointState
				^ (((*puState & MUB_MASK) | (*puNextState & NUB_MASK)) >> 1)
				^ ((*puNextState & MLB_MASK) ? MAGIC_MASK_A : 0);

			puState++;
			puNextState++;
			puPointState++;
		}

		puEndState = m_auState + (STATES - 1);
		puPointState = puState + (PERIOD_STATE - STATES);

		while (puState < puEndState) {

			*puState =
				*puPointState
				^ (((*puState & MUB_MASK) | (*puNextState & NUB_MASK)) >> 1)
				^ ((*puNextState & MLB_MASK) ? MAGIC_MASK_A : 0);

			puState++;
			puNextState++;
			puPointState++;
		}

		*puState = *puPointState
			^ (((*puState & MUB_MASK) | (*m_auState & NUB_MASK)) >> 1)
			^ ((*m_auState & MLB_MASK) ? MAGIC_MASK_A : 0);

		m_puNextState = m_auState;
	}

	uState = *m_puNextState;
	m_puNextState++;
	uState ^= (uState >> MAGIC_RSHIFT_NUMBER_U);
	uState ^= (uState << MAGIC_LSHIFT_NUMBER_S) & MAGIC_MASK_B;
	uState ^= (uState << MAGIC_LSHIFT_NUMBER_T) & MAGIC_MASK_C;
	return (uState ^ (uState >> MAGIC_RSHIFT_NUMBER_L));
}

u8 FASTCALL FncMtRandU8(void) {
	if (!m_uShiftCounter8) {
		m_uShiftCounter8 = 24;
		m_uCurrentNumber8 = FncMtRand();
	} else {
		m_uShiftCounter8 -= 8;
	}
	return (u8)((m_uCurrentNumber8 >> m_uShiftCounter8) & 0xff);
}

u16 FASTCALL FncMtRandU16(void) {
	if (!m_uShiftCounter16) {
		m_uShiftCounter16 = 16;
		m_uCurrentNumber16 = FncMtRand();
	} else {
		m_uShiftCounter16 -= 16;
	}
	return (u16)((m_uCurrentNumber16 >> m_uShiftCounter16) & 0xffff);
}

u64 FASTCALL FncMtRandU64(void) {
	return (u64)(((u64)FncMtRand() << 32) | FncMtRand());
}

f32 FASTCALL FncMtRandR32(void) {
	return (f32)(U32_TO_R32_LT_1 * FncMtRand());
}

f64 FASTCALL FncMtRandR64(void) {
	return (f64)(U32_TO_R64_LT_1 * FncMtRand());
}