/*! \file mu.h

*  \brief guVec inline assembly replacements
*/
#pragma once

#ifndef _MU_H
#define _MU_H

#include <ogc/gu.h>

static f32 NrmData[] = { 0.5, 3.0 };
static f32 RandMad[] = { 2.0f, -1.0f };

static inline void muVecAdd(register guVector *a, register guVector *b, register guVector *ab) {
	register f32 f0, f1, f2, f3, f4, f5;
	asm (
		"psq_l		%[f0], 0(%[a]), 0, 0;"
		"psq_l		%[f1], 0(%[b]), 0, 0;"
		"ps_add		%[f2], %[f0], %[f1];"
		"psq_st		%[f2], %[ab], 0, 0;"
		"psq_l		%[f3], 8(%[a]), 1, 0;"
		"psq_l		%[f4], 8(%[b]), 1, 0;"
		"ps_add		%[f5], %[f3], %[f4];"
		"psq_st		%[f5], 8+%[ab], 1, 0;"
		: [ab] "=o>" (*ab),
		  [f0] "=&fr" (f0),
		  [f1] "=&fr" (f1),
		  [f2] "=&fr" (f2),
		  [f3] "=&fr" (f3),
		  [f4] "=&fr" (f4),
		  [f5] "=&fr" (f5)
		: [a] "r"(a), [b] "r"(b)
	);
}

static inline void muVecSub(register guVector *a, register guVector *b, register guVector *ab) {
	register f32 f0, f1, f2, f3, f4, f5;
	asm(
		"psq_l		%[f0], 0(%[a]), 0, 0;"
		"psq_l		%[f1], 0(%[b]), 0, 0;"
		"ps_sub		%[f2], %[f0], %[f1];"
		"psq_st		%[f2], %[ab], 0, 0;"
		"psq_l		%[f3], 8(%[a]), 1, 0;"
		"psq_l		%[f4], 8(%[b]), 1, 0;"
		"ps_sub		%[f5], %[f3], %[f4];"
		"psq_st		%[f5], 8+%[ab], 1, 0;"
		: [ab] "=o>" (*ab),
		  [f0] "=&fr" (f0),
		  [f1] "=&fr" (f1),
		  [f2] "=&fr" (f2),
		  [f3] "=&fr" (f3),
		  [f4] "=&fr" (f4),
		  [f5] "=&fr" (f5)
		: [a] "r"(a), [b] "r"(b)
	);
}

static inline void muVecScale(register guVector *src, register guVector *dst, f32 scale) {
	register f32 i0, i1, i2;
	asm (
		"psq_l		%[i0], 0(%[src]), 0, 0;"
		"psq_l		%[i1], 8(%[src]), 1, 0;"
		"ps_muls0	%[i2], %[i0], %[scale];"
		"psq_st		%[i2], %[dst], 0, 0;"
		"ps_muls0	%[i2], %[i1], %[scale];"
		"psq_st		%[i2], 8 + %[dst], 1, 0;"
		: [dst] "=o>" (*dst),
		  [i0] "=&fr" (i0),
		  [i1] "=&fr" (i1),
		  [i2] "=&fr" (i2)
		: [src] "r"(src), [scale] "f"(scale)
	);
}

static inline void muVecInvert(register guVector *src, register guVector *dst) {
	register f32 i0, i1, i2;
	asm(
		"psq_l		%[i0], 0(%[src]), 0, 0;"
		"psq_l		%[i1], 8(%[src]), 1, 0;"
		"ps_neg		%[i2], %[i0];"
		"psq_st		%[i2], %[dst], 0, 0;"
		"ps_neg		%[i2], %[i1];"
		"psq_st		%[i2], 8 + %[dst], 1, 0;"
		: [dst] "=o>" (*dst),
		[i0] "=&fr" (i0),
		[i1] "=&fr" (i1),
		[i2] "=&fr" (i2)
		: [src] "r"(src)
		);
}

//TODO: Wasting a register on NrmData adress, find out how to use directly from memory!
static inline void muVecNormalize(register guVector *v) {
	register f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9;
	asm (
		"lfs		%[f0], %[data];"
		"lfs		%[f1], 4 + %[data];"
		"psq_l		%[f2], %[v], 0, 0;"
		"ps_mul		%[f4], %[f2], %[f2];"
		"psq_l		%[f3], 8 + %[v], 1, 0;"
		"ps_madd	%[f5], %[f3], %[f3], %[f4];"
		"ps_sum0	%[f6], %[f5], %[f3], %[f4];"
		"frsqrte	%[f7], %[f6];"
		"fmuls		%[f8], %[f7], %[f7];"
		"fmuls		%[f9], %[f7], %[f0];"
		"fnmsubs	%[f8], %[f8], %[f6], %[f1];"
		"fmuls		%[f7], %[f8], %[f9];"
		"ps_muls0	%[f2], %[f2], %[f7];"
		"psq_st		%[f2], %[v], 0, 0;"
		"ps_muls0	%[f3], %[f3], %[f7];"
		"psq_st		%[f3], 8 + %[v], 1, 0;"
		: [v] "+o>" (*v),
		  [f0] "=&fr" (f0),
		  [f1] "=&fr" (f1),
		  [f2] "=&fr" (f2),
		  [f3] "=&fr" (f3),
		  [f4] "=&fr" (f4),
		  [f5] "=&fr" (f5),
		  [f6] "=&fr" (f6),
		  [f7] "=&fr" (f7),
		  [f8] "=&fr" (f8),
		  [f9] "=&fr" (f9)
		: [data] "m>" (NrmData)
	);
}

static inline void muVecCross(register guVector *a, register guVector *b, register guVector *axb) {
	register f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10;
	asm (
		"psq_l		%[f1], 0(%[b]), 0, 0;"
		"lfs		%[f2], 8(%[a]);"
		"psq_l		%[f0], 0(%[a]), 0, 0;"
		"ps_merge10	%[f6], %[f1], %[f1];"
		"lfs		%[f3], 8(%[b]);"
		"ps_mul		%[f4], %[f1], %[f2];"
		"ps_muls0	%[f7], %[f1], %[f0];"
		"ps_msub	%[f5], %[f0], %[f3], %[f4];"
		"ps_msub	%[f8], %[f0], %[f6], %[f7];"
		"ps_merge11	%[f9], %[f5], %[f5];"
		"ps_merge01	%[f10], %[f5], %[f8];"
		"psq_st		%[f9], %[axb], 1, 0;"
		"ps_neg		%[f10], %[f10];"
		"psq_st		%[f10], 4 + %[axb], 0, 0;"
		: [axb] "=o>"(*axb),
		  [f0] "=&fr" (f0),
		  [f1] "=&fr" (f1),
		  [f2] "=&fr" (f2),
		  [f3] "=&fr" (f3),
		  [f4] "=&fr" (f4),
		  [f5] "=&fr" (f5),
		  [f6] "=&fr" (f6),
		  [f7] "=&fr" (f7),
		  [f8] "=&fr" (f8),
		  [f9] "=&fr" (f9),
		  [f10] "=&fr" (f10)
		: [a] "r"(a), [b] "r"(b)
	);
}

static inline void muVecMultiply(register Mtx mt, register guVector *src, register guVector *dst) {
	register f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12;
	asm (
		"psq_l		%[f0], 0(%[src]), 0, 0;"
		"psq_l		%[f2], 0(%[mt]), 0, 0;"
		"psq_l		%[f1], 8(%[src]), 1, 0;"
		"ps_mul		%[f4], %[f2], %[f0];"
		"psq_l		%[f3], 8(%[mt]), 0, 0;"
		"ps_madd	%[f5], %[f3], %[f1], %[f4];"
		"psq_l		%[f8], 16(%[mt]), 0, 0;"
		"ps_sum0	%[f6], %[f5], %[f6], %[f5];"
		"psq_l		%[f9], 24(%[mt]), 0, 0;"
		"ps_mul		%[f10], %[f8], %[f0];"
		"psq_st		%[f6], %[dst], 1, 0;"
		"ps_madd	%[f11], %[f9], %[f1], %[f10];"
		"psq_l		%[f2], 32(%[mt]), 0, 0;"
		"ps_sum0	%[f12], %[f11], %[f12], %[f11];"
		"psq_l		%[f3], 40(%[mt]), 0, 0;"
		"ps_mul		%[f4], %[f3], %[f0];"
		"psq_st		%[f12], 4 + %[dst], 1, 0;"
		"ps_madd	%[f5], %[f3], %[f1], %[f4];"
		"ps_sum0	%[f6], %[f5], %[f6], %[f5];"
		"psq_st		%[f6], 8 + %[dst], 1, 0;"
		: [dst] "=o>"(*dst),
		[f0] "=&fr" (f0),
		[f1] "=&fr" (f1),
		[f2] "=&fr" (f2),
		[f3] "=&fr" (f3),
		[f4] "=&fr" (f4),
		[f5] "=&fr" (f5),
		[f6] "=&fr" (f6),
		[f7] "=&fr" (f7),
		[f8] "=&fr" (f8),
		[f9] "=&fr" (f9),
		[f10] "=&fr" (f10),
		[f11] "=&fr" (f11),
		[f12] "=&fr" (f12)
		: [mt] "r"(mt), [src] "r"(src)
	);
}

static inline void muVecMultiplySR(register Mtx mt, register guVector *src, register guVector *dst) {
	register f32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13;
	asm (
		"psq_l		%[f0], 0(%[mt]), 0, 0;"
		"psq_l		%[f6], 0(%[src]), 0, 0;"
		"psq_l		%[f2], 16(%[mt]), 0, 0;"
		"ps_mul		%[f8], %[f0], %[f6];"
		"psq_l		%[f4], 32(%[mt]), 0, 0;"
		"ps_mul		%[f10], %[f2], %[f6];"
		"psq_l		%[f7], 8(%[src]), 1, 0;"
		"ps_mul		%[f12], %[f4], %[f6];"
		"psq_l		%[f3], 24(%[mt]), 0, 0;"
		"ps_sum0	%[f8], %[f8], %[f8], %[f8];"
		"psq_l		%[f5], 40(%[mt]), 0, 0;"
		"ps_sum0	%[f10], %[f10], %[f10], %[f10];"
		"psq_l		%[f1], 8(%[mt]), 0, 0;"
		"ps_sum0	%[f12], %[f12], %[f12], %[f12];"
		"ps_madd	%[f9], %[f1], %[f7], %[f8];"
		"psq_st		%[f9], %[dst], 1, 0 ;"
		"ps_madd	%[f11], %[f3], %[f7], %[f12];"
		"psq_st		%[f11], 4 %[dst], 1, 0 ;"
		"ps_madd	%[f13], %[f5], %[f7], %[f12];"
		"psq_st		%[f13], 8 + %[dst], 1, 0;"
		: [dst] "=o>"(*dst),
		[f0] "=&fr" (f0),
		[f1] "=&fr" (f1),
		[f2] "=&fr" (f2),
		[f3] "=&fr" (f3),
		[f4] "=&fr" (f4),
		[f5] "=&fr" (f5),
		[f6] "=&fr" (f6),
		[f7] "=&fr" (f7),
		[f8] "=&fr" (f8),
		[f9] "=&fr" (f9),
		[f10] "=&fr" (f10),
		[f11] "=&fr" (f11),
		[f12] "=&fr" (f12),
		[f13] "=&fr" (f13)
		: [mt] "r"(mt), [src] "r"(src)
	);
}

static inline f32 muVecDotProduct(register guVector *a, register guVector *b) {
	register f32 result;
	register f32 f0, f1, f2, f3;
	asm (
		"psq_l      %[f0], 4(%[a]), 0, 0;"
		"psq_l      %[f1], 4(%[b]), 0, 0;"
		"ps_mul     %[f0], %[f0], %[f1];"
		"psq_l      %[f2], 0(%[a]), 0, 0;"
		"psq_l      %[f3], 0(%[b]), 0, 0;"
		"ps_madd    %[f1], %[f2], %[f3], %[f0];"
		"ps_sum0    %[res], %[f1], %[f0], %[f0];"
		: [res] "=f"(result),
		  [f0] "=&fr" (f0),
		  [f1] "=&fr" (f1),
		  [f2] "=&fr" (f2),
		  [f3] "=&fr" (f3)
		: [a]"r"(a), [b]"r"(b)
	);
	return result;
}

static inline f32 muSqrtf(register float val) {
	register float res, tmp;
	asm ("frsqrte %[tmp], %[in];"
		"fres %[out], %[tmp];"
		: [out] "=fr" (res),
		  [tmp] "=&fr" (tmp)
		: [in] "f" (val)
	);
	return res;
}

// r3 = vec1, r4 = vec2, r5 = rand, r6 = mad
static inline void muRandScale(register guVector *a, register guVector *b, register f32* rand) {
	register f32 f0, f1, f2, f3, f4, f5;
	asm(
		"psq_l		%[f0], 0(%[rand]), 0, 0;"
		"psq_l		%[f1], %[a], 0, 0;"
		"psq_l		%[f2], 8 + %[a], 1, 0;"
		"psq_l		%[f3], 0(%[mad]), 0, 0;"
		"ps_merge11	%[f4], %[f3], %[f3];"
		"ps_madds0	%[f0], %[f0], %[f3], %[f4];"
		"ps_muls0	%[f1], %[f1], %[f0];"
		"psq_st		%[f1], %[a], 0, 0;"
		"ps_muls0	%[f2], %[f2], %[f0];"
		"psq_st		%[f2], 8 + %[a], 1, 0;"

		"psq_l		%[f1], %[b], 0, 0;"
		"psq_l		%[f2], 8 + %[b], 1, 0;"
		"ps_muls1	%[f1], %[f1], %[f0];"
		"psq_st		%[f1], %[b], 0, 0;"
		"ps_muls1	%[f2], %[f2], %[f0];"
		"psq_st		%[f2], 8 + %[b], 1, 0;"
		: [a] "+o>" (*a), [b] "+o>" (*b),
		[f0] "=&fr" (f0),
		[f1] "=&fr" (f1),
		[f2] "=&fr" (f2),
		[f3] "=&fr" (f3),
		[f4] "=&fr" (f4),
		[f5] "=&fr" (f5)
		: [rand] "r" (rand), [mad] "r" (RandMad)
	);
}

#endif
