/*! \file mu.h

*  \brief guVec inline assembly replacements
*/
#pragma once

#ifndef _MU_H
#define _MU_H

#include <ogc/gu.h>

extern float NrmData[];

#define muVecAdd(a, b, ab)	\
	asm(	\
		"psq_l		2, 0(%[_a]), 0, 0;"	\
		"psq_l		4, 0(%[_b]), 0, 0;"	\
		"ps_add		6, 2, 4;"	\
		"psq_st		6, 0(%[_ab]), 0, 0;"	\
		"psq_l		3, 8(%[_a]), 1, 0;"	\
		"psq_l		5, 8(%[_b]), 1, 0;"	\
		"ps_add		7, 3, 5;"	\
		"psq_st		7, 8(%[_ab]), 1, 0;"	\
		:	\
		: [_a] "r"(a), [_b] "r"(b), [_ab] "r"(ab)	\
		: "memory", "2", "3", "4", "5", "6", "7"	\
	)

#define muVecSub(a, b,ab)	\
	asm(	\
		"psq_l		2, 0(%[_a]), 0, 0;"	\
		"psq_l		4, 0(%[_b]), 0, 0;"	\
		"ps_sub		6, 2, 4;"	\
		"psq_st		6, 0(%[_ab]), 0, 0;"	\
		"psq_l		3, 8(%[_a]), 1, 0;"	\
		"psq_l		5, 8(%[_b]), 1, 0;"	\
		"ps_sub		7, 3, 5;"	\
		"psq_st		7, 8(%[_ab]), 1, 0;"	\
		:	\
		: [_a] "r"(a), [_b] "r"(b), [_ab] "r"(ab)	\
		: "memory", "2", "3", "4", "5", "6", "7"	\
	)

#define muVecDotProduct2(a, b, sum) \
{ \
      register f32 inter0; \
      register f32 inter1; \
      register f32 inter2; \
      register f32 inter3; \
      __asm__ ( \
            "psq_l            %[int1], 4(%[_a]), 0, 0;" \
            "psq_l            %[int2], 4(%[_b]), 0, 0;" \
            "ps_mul           %[int1], %[int1], %[int2];" \
            "psq_l            %[int3], 0(%[_a]), 0, 0;" \
            "psq_l            %[int4], 0(%[_b]), 0, 0;" \
            "ps_madd    %[int2], %[int3], %[int4], %[int1];" \
            "ps_sum0    %[res], %[int2], %[int1], %[int1];" \
            : [res] "=f"(sum), \
              [int1] "=f" (inter0), \
              [int2] "=f" (inter1), \
              [int3] "=f" (inter2), \
              [int4] "=f" (inter3) \
            : [_a]"r"(a), [_b]"r"(b) \
      ); \
}

//extern void muVecAdd(register guVector *a, register guVector *b, register guVector *ab);
//extern void muVecSub(register guVector *a, register guVector *b, register guVector *ab);
void muVecScale(register guVector *src, register guVector *dst, f32 scale);
void muVecNormalize(register guVector *v);
void muVecCross(register guVector *a, register guVector *b, register guVector *axb);
void muVecMultiply(register Mtx mt, register guVector *src, register guVector *dst);
void muVecMultiplySR(register Mtx mt, register guVector *src, register guVector *dst);
f32 muVecDotProduct(register guVector *a, register guVector *b);

#endif
