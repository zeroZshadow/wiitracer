/* ==========================================================================

nc_mtrandom.h

Mersenne Twister random number generator
Based on code by Makoto Matsumoto, Takuji Nishimura

Reference
M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

========================================================================== */

#ifndef NC_MTRANDOM_H
#define NC_MTRANDOM_H

#include <ogc/gu.h>

#define FLT_EPSILON 1.19209290E-07F
#define DBL_EPSILON 2.2204460492503131E-16
#define	U32_TO_R32_LT_1 ((f32)((1.0-FLT_EPSILON)/0xffffffff))
#define	U32_TO_R64_LT_1 ((f64)((1.0-DBL_EPSILON)/0xffffffff))

#if defined(_MSC_VER) || defined(__BORLANDC__)
/* Microsoft or Borland C C/C++ Compiler */
#define FASTCALL   __fastcall
#define DECCALL  __cdecl

#else
/* Etc C/C++ Compilers */
#define FASTCALL
#define DECCALL

#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void	FASTCALL FncMtSrand(u32 uSeed);
void	FASTCALL FncMtSrands(u32 const* auSeed, u32 uSeeds);
void	FASTCALL FncMtRandomize(void);
u32		FASTCALL FncMtRand(void);
u8		FASTCALL FncMtRandU8(void);
u16		FASTCALL FncMtRandU16(void);
#define	FncMtRandU32() FncMtRand()
u64		FASTCALL FncMtRandU64(void);
f32		FASTCALL FncMtRandR32(void);
f64		FASTCALL FncMtRandR64(void);
int		FASTCALL FncMtSerialIn(int iFdIn);
int		FASTCALL FncMtSerialOut(int iFdOut);
int		FASTCALL FncMtSerialInFile(char const* pszFileName);
int		FASTCALL FncMtSerialOutFile(char const* pszFileName);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
