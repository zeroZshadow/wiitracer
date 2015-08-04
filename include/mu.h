/*! \file mu.h
*  \brief guVec inline assembly replacements
*/

#ifndef _MU_H
#define _MU_H

#include <ogc/gu.h>

inline void muVecAdd(register guVector *a, register guVector *b, register guVector *ab);
inline void muVecSub(register guVector *a, register guVector *b, register guVector *ab);
inline void muVecScale(register guVector *src, register guVector *dst, f32 scale);
inline void muVecNormalize(register guVector *v);
inline void muVecCross(register guVector *a, register guVector *b, register guVector *axb);
inline void muVecMultiply(register Mtx mt, register guVector *src, register guVector *dst);
inline void muVecMultiplySR(register Mtx mt, register guVector *src, register guVector *dst);
inline f32 muVecDotProduct(register guVector *a, register guVector *b);

#endif
