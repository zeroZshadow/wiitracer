/*! \file mu.h

*  \brief guVec inline assembly replacements
*/
#pragma once

#ifndef _MU_H
#define _MU_H

#include <ogc/gu.h>

static float NrmData[] = { 0.5, 3.0 };

static inline void muVecAdd(register guVector *a, register guVector *b, register guVector *ab) {
	asm(
		"psq_l		2, 0(%[a]), 0, 0;"
		"psq_l		4, 0(%[b]), 0, 0;"
		"ps_add		6, 2, 4;"
		"psq_st		6, 0(%[ab]), 0, 0;"
		"psq_l		3, 8(%[a]), 1, 0;"
		"psq_l		5, 8(%[b]), 1, 0;"
		"ps_add		7, 3, 5;"
		"psq_st		7, 8(%[ab]), 1, 0;"
		:
		: [a] "r"(a), [b] "r"(b), [ab] "r"(ab)
		: "memory", "2", "3", "4", "5", "6", "7"
	);
}

static inline void muVecSub(register guVector *a, register guVector *b, register guVector *ab) {
	asm(
		"psq_l		2, 0(%[a]), 0, 0;"
		"psq_l		4, 0(%[b]), 0, 0;"
		"ps_sub		6, 2, 4;"
		"psq_st		6, 0(%[ab]), 0, 0;"
		"psq_l		3, 8(%[a]), 1, 0;"
		"psq_l		5, 8(%[b]), 1, 0;"
		"ps_sub		7, 3, 5;"
		"psq_st		7, 8(%[ab]), 1, 0;"
		:
		: [a] "r"(a), [b] "r"(b), [ab] "r"(ab)
		: "memory", "2", "3", "4", "5", "6", "7"
	);
}

static inline void muVecScale(register guVector *src, register guVector *dst, f32 scale) {
	asm(
		"psq_l		2, 0(%[src]), 0, 0;"
		"psq_l		3, 8(%[src]), 1, 0;"
		"ps_muls0	4, 2, %[scale];"
		"psq_st		4, 0(%[dst]), 0, 0;"
		"ps_muls0	4, 3, %[scale];"
		"psq_st		4, 8(%[dst]), 1, 0;"
		:
		: [src] "r"(src), [dst] "r"(dst), [scale] "f"(scale)
		: "memory", "2", "3", "4"
	);
}

//TODO: Wasting a register on NrmData adress, find out how to use directly from memory!
static inline void muVecNormalize(register guVector *v) {
	asm(
		"lfs		0, 0(%[data]);"
		"lfs		1, 4(%[data]);"
		"psq_l		2, 0(%[v]), 0, 0;"
		"ps_mul		4, 2, 2;"
		"psq_l		3, 8(%[v]), 1, 0;"
		"ps_madd	5, 3, 3, 4;"
		"ps_sum0	6, 5, 3, 4;"
		"frsqrte	7, 6;"
		"fmuls		8, 7, 7;"
		"fmuls		9, 7, 0;"
		"fnmsubs	8, 8, 6, 1;"
		"fmuls		7, 8, 9;"
		"ps_muls0	2, 2, 7;"
		"psq_st		2, 0(%[v]), 0, 0;"
		"ps_muls0	3, 3, 7;"
		"psq_st		3, 8(%[v]), 1, 0;"
		:
		: [v] "r"(v), [data] "r"(&NrmData)
		: "memory", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
	);
}

static inline void muVecCross(register guVector *a, register guVector *b, register guVector *axb) {
	asm(
		"psq_l		1, 0(%[b]), 0, 0;"
		"lfs		2, 8(%[a]);"
		"psq_l		0, 0(%[a]), 0, 0;"
		"ps_merge10	6, 1, 1;"
		"lfs		3, 8(%[b]);"
		"ps_mul		4, 1, 2;"
		"ps_muls0	7, 1, 0;"
		"ps_msub	5, 0, 3, 4;"
		"ps_msub	8, 0, 6, 7;"
		"ps_merge11	9, 5, 5;"
		"ps_merge01	10, 5, 8;"
		"psq_st		9, 0(%[axb]), 1, 0;"
		"ps_neg		10, 10;"
		"psq_st		10, 4(%[axb]), 0, 0;"
		:
		: [a] "r"(a), [b] "r"(b), [axb] "r"(axb)
		: "memory", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
	);
}

static inline void muVecMultiply(register Mtx mt, register guVector *src, register guVector *dst) {
	asm(
		"psq_l		0, 0(%[src]), 0, 0;"
		"psq_l		2, 0(%[mt]), 0, 0;"
		"psq_l		1, 8(%[src]), 1, 0;"
		"ps_mul		4, 2, 0;"
		"psq_l		3, 8(%[mt]), 0, 0;"
		"ps_madd	5, 3, 1, 4;"
		"psq_l		8, 16(%[mt]), 0, 0;"
		"ps_sum0	6, 5, 6, 5;"
		"psq_l		9, 24(%[mt]), 0, 0;"
		"ps_mul		10, 8, 0;"
		"psq_st		6, 0(%[dst]), 1, 0;"
		"ps_madd	11, 9, 1, 10;"
		"psq_l		2, 32(%[mt]), 0, 0;"
		"ps_sum0	12, 11, 12, 11;"
		"psq_l		3, 40(%[mt]), 0, 0;"
		"ps_mul		4, 3, 0;"
		"psq_st		12, 4(%[dst]), 1, 0;"
		"ps_madd	5, 3, 1, 4;"
		"ps_sum0	6, 5, 6, 5;"
		"psq_st		6, 8(%[dst]), 1, 0;"
		:
		: [mt] "r"(mt), [src] "r"(src), [dst] "r"(dst)
		: "memory", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"
	);
}

static inline void muVecMultiplySR(register Mtx mt, register guVector *src, register guVector *dst) {
	asm(
		"psq_l		0, 0(%[mt]), 0, 0;"
		"psq_l		6, 0(%[src]), 0, 0;"
		"psq_l		2, 16(%[mt]), 0, 0;"
		"ps_mul		8, 0, 6;"
		"psq_l		4, 32(%[mt]), 0, 0;"
		"ps_mul		10, 2, 6;"
		"psq_l		7, 8(%[src]), 1, 0;"
		"ps_mul		12, 4, 6;"
		"psq_l		3, 24(%[mt]), 0, 0;"
		"ps_sum0	8, 8, 8, 8;"
		"psq_l		5, 40(%[mt]), 0, 0;"
		"ps_sum0	10, 10, 10, 10;"
		"psq_l		1, 8(%[mt]), 0, 0;"
		"ps_sum0	12, 12, 12, 12;"
		"ps_madd	9, 1, 7, 8;"
		"psq_st		9, 0(%[dst]), 1, 0 ;"
		"ps_madd	11, 3, 7, 10;"
		"psq_st		11, 4(%[dst]), 1, 0 ;"
		"ps_madd	13, 5, 7, 12;"
		"psq_st		13, 8(%[dst]), 1, 0;"
		:
		: [mt] "r"(mt), [src] "r"(src), [dst] "r"(dst)
		: "memory", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13"
	);
}

static inline f32 muVecDotProduct(register guVector *a, register guVector *b) {
	register f32 result;
	register f32 inter0, inter1, inter2, inter3;
	__asm__(
		"psq_l            %[int1], 4(%[a]), 0, 0;"
		"psq_l            %[int2], 4(%[b]), 0, 0;"
		"ps_mul           %[int1], %[int1], %[int2];"
		"psq_l            %[int3], 0(%[a]), 0, 0;"
		"psq_l            %[int4], 0(%[b]), 0, 0;"
		"ps_madd    %[int2], %[int3], %[int4], %[int1];"
		"ps_sum0    %[res], %[int2], %[int1], %[int1];"
		: [res] "=f"(result),
		[int1] "=&f" (inter0),
		[int2] "=&f" (inter1),
		[int3] "=&f" (inter2),
		[int4] "=&f" (inter3)
		: [a]"r"(a), [b]"r"(b)
	);
	return result;
}

#endif
