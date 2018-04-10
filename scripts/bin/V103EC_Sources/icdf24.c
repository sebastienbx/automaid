/**************************************************************************//**
 * @file cdf24.c
 * @version 0.99
 * @date jeu. mai 14 00:37:45 CEST 2009
 *
 * @author jf.argentino@osean.fr
 * @brief 
 *
 * history: 
 *          1.01: initial release
 ******************************************************************************/

#include "icdf24.h"
#include "long_ops.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef DBG_CDF24
   #ifdef MERMAID_ECOS_BUILD
   #include "../Message.h"
   #define DBG_PRINT(x, ...) write_DBG("ICDF24", __LINE__, x , ##__VA_ARGS__);
   #else /* MERMAID_ECOS_BUILD not defined */
   #define DBG_PRINT(x, ...) printf("ICDF24 %d: " x "\n", __LINE__,\
                                    ##__VA_ARGS__);
   #endif /* MERMAID_ECOS_BUILD */
#else
   #define DBG_PRINT(x, ...)
#endif

// this line is commented by me; the arrays _a and _d are declared inside each function below;
//int32_t _a[CDF24_MAX_LENGTH >> 1], _d[CDF24_MAX_LENGTH >> 1];

/******************************************************************************
 * TODO renaming constants, we have
 * TODO    - Pa = [1 1] / 2
 * TODO    - Ua = [-3 19 19 -3] / 64
 * TODO    - Kp = SQRT (2)
 * TODO    - Ku = 1 / SQRT (2)
 ******************************************************************************/
/*
 * SQRT(2) = 665857 / 470832 = 577 / 408 = 239 / 169 = 99 / 70 = 41 / 29 ...
 */
static const int32_t SQRT2_NUM = 239L;
static const int32_t SQRT2_DEN = 169L;
static const int32_t A = 3L;
static const int32_t B = 19L;
static const int32_t C = 64L;

static void normalized_sqrt2_cdf24 (int32_t* x, size_t lx, size_t K) {
   size_t k, n, m, i;

   // my addition - arrays _a and _d are allocated inside the function
   int32_t* _a;
   int32_t* _d;
	_a = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
	_d = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
  ///////////////////////////////////////////////////////////////////
    
    DBG_PRINT ("unnormalized on %zu@%p", lx, (void*) x);
      lx >>= K; // dividing lx by 2^K;

   for (k = K; k > 0UL; k--) {
      DBG_PRINT ("round %zu on %zu", k, K - 1U);
    
     /* Undoing Normalization */ 
      for (n = 0; n < lx; n++) {
	 // first half of the vector 1:2*lx is approximation vector - dividing by sqrt(2)
         x[n] = long_mult_and_div (x[n], SQRT2_DEN, SQRT2_NUM);
	 // second half of the vector 1:2*lx is details vector - multiplying by sqrt(2)
         x[n+lx] = long_mult_and_div (x[n+lx], SQRT2_NUM, SQRT2_DEN);
      }

      /* Approximation part */
      // edge corrections
      // Haar transform
      x[0] = x[0] - long_mult_and_div (x[lx], 1L, 2L);
      _a[0] = x[0];

      if (lx > 1) {  
      // cdf(2,2) -- applied in all cases except when lx=1, i.e. there are only 2 points in the vector; this means that points 
      // a[0] and d[0] were calculated with the Haar transform only, which is applied before; also note, that x[1] should be 
      // overwritten with a new value because it is used later to inverse values of d vector; 
      x[1] = x[1] - long_mult_and_div (x[lx] + x[lx+1], 1L, 4L);
      _a[1] = x[1]; }

      for (n = 2UL; n < lx - 1UL; n ++) {
         m = n;
         x[m] += long_mult_and_div (x[lx+m-2UL] + x[lx+m+1UL], A, C);
         x[m] -= long_mult_and_div (x[lx+m-1UL] + x[lx+m], B, C);
         _a[n] = x[m];
      }

      if (lx > 2) {  
      // edge correction cdf(2,2) -- applied only when lx = 3 and more, i.e. it is skipped for the case when vector contains 
      // 4 points, a[0] a[1] d[0] d[1], i.e. lx =2; this is required because this condition becomes identical to condition for _a[1] above
      // when lx=2; in this case, x[1] becomes updated twice and therefore _a[1] = x[lx-1] -2*(x[(lx<<1)-1] + x[(lx<<1)-2])/ 4;
      x[lx-1] = x[lx-1] - long_mult_and_div (x[(lx<<1)-1] + x[(lx<<1)-2], 1L, 4L);
      _a[lx-1] = x[lx-1];}

      /* Detail part */
      for (n = 0UL; n < lx-1; n++) {
         m = n;
         x[m+lx] += (x[m] + x[m+1UL]) / 2L;
         _d[n] = x[m+lx];
      }
      // edge correction Haar transform
      x[(lx<<1) - 1] = x[(lx<<1) - 1] + x[lx - 1];
      _d[lx-1] = x[(lx<<1) - 1];

      /* Resort */
      for (n = 0; n < lx; n++) {
         x[(n<<1)] = _a[n];
         x[(n<<1)+1] = _d[n];
      }

      lx <<= 1; // after each scale undone, the length is increased by 2
   }
    
    //Si K!= de 6 diviser le signal par [ (2^(1/2)) * (6-K) ]
    for (i = 0; i<6-K; i++) {
        for (n = 0; n < lx; n++) {
            // first half of the vector 1:2*lx is approximation vector - dividing by sqrt(2)
            x[n] = long_mult_and_div (x[n], SQRT2_DEN, SQRT2_NUM);
        }
    }
    
  }

static void normalized_2_cdf24 (int32_t* x, size_t lx, size_t K) {
   size_t k, n, m, q;
   // my addition - arrays _a and _d are allocated inside the function
   int32_t* _a;
   int32_t* _d;
	_a = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
	_d = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
  ///////////////////////////////////////////////////////////////////

   DBG_PRINT ("unnormalized on %zu@%p", lx, (void*) x);
     lx >>= K; // dividing lx by 2^K;
     q = 0; 

   for (k = K; k > 0UL; k--) {
      DBG_PRINT ("round %zu on %zu", k, K - 1U);
     /* Undoing Normalization */ 
      for (n = 0; n < lx; n++) {
	if (~q % 2)
	{ // first half of the vector 1:2*lx is approximation vector - dividing by 2 if q is even
		// vector d is not changed
         x[n] >>= 1; //long_mult_and_div (x[n], SQRT2_DEN, SQRT2_NUM);
	}
	else
	{ // second half of the vector 1:2*lx is details vector - multiplying by sqrt(2) if q is odd
		// a is not changed
         x[n+lx] = long_mult_and_div (x[n+lx], SQRT2_NUM, SQRT2_DEN);
	}
      }
      /* Approximation part */
      // edge corrections
      // Haar transform
      x[0] = x[0] - long_mult_and_div (x[lx], 1L, 2L);
      _a[0] = x[0];

      if (lx > 1) {  
      // cdf(2,2) -- applied in all cases except when lx=1, i.e. there are only 2 points in the vector; this means that points 
      // a[0] and d[0] were calculated with the Haar transform only, which is applied before; also note, that x[1] should be 
      // overwritten with a new value because it is used later to inverse values of d vector; 
      x[1] = x[1] - long_mult_and_div (x[lx] + x[lx+1], 1L, 4L);
      _a[1] = x[1]; }

      for (n = 2UL; n < lx - 1UL; n ++) {
         m = n;
         x[m] += long_mult_and_div (x[lx+m-2UL] + x[lx+m+1UL], A, C);
         x[m] -= long_mult_and_div (x[lx+m-1UL] + x[lx+m], B, C);
         _a[n] = x[m];
      }

      if (lx > 2) {  
      // edge correction cdf(2,2) -- applied only when lx = 3 and more, i.e. it is skipped for the case when vector contains 
      // 4 points, a[0] a[1] d[0] d[1], i.e. lx =2; this is required because this condition becomes identical to condition for _a[1] above
      // when lx=2; in this case, x[1] becomes updated twice and therefore _a[1] = x[lx-1] -2*(x[(lx<<1)-1] + x[(lx<<1)-2])/ 4;
      x[lx-1] = x[lx-1] - long_mult_and_div (x[(lx<<1)-1] + x[(lx<<1)-2], 1L, 4L);
      _a[lx-1] = x[lx-1];}

      /* Detail part */
      for (n = 0UL; n < lx-1; n++) {
         m = n;
         x[m+lx] += (x[m] + x[m+1UL]) / 2L;
         _d[n] = x[m+lx];
      }
      // edge correction Haar transform
      x[(lx<<1) - 1] = x[(lx<<1) - 1] + x[lx - 1];
      _d[lx-1] = x[(lx<<1) - 1];

      /* Resort */
      for (n = 0; n < lx; n++) {
         x[(n<<1)] = _a[n];
         x[(n<<1)+1] = _d[n];
      }

      lx <<= 1; // after each scale undone, the length is increased by 2
      q ++;
   }
   // if the number of scales is odd, it is necessary to multiply final vector by sqrt(2)
   // this is because normalization by dividing vector a by 2 assumes that the vector a obtained
   // after undoing one scale of the wavelet transform is already divided by sqrt(2); but if
   // on this vector the inverse wt stops, then we don't need to divide by sqrt(2); so this division 
   // should be undone; if K is even, the vector a is just as if it was normalized in a usual manner by
   // dividing by sqrt(2) at each step; so it is either divided by 2 if the inverse wt is continued, or simply left as is,
   // if inverse wt is finished on it
   if (K % 2 ) {
      for (n = 0; n < lx; n++) { x[n] = long_mult_and_div (x[n], SQRT2_NUM, SQRT2_DEN);}
   }
}

static void unnormalized_cdf24 (int32_t* x, size_t lx, size_t K) {
   size_t k, n, m;
   // my addition - arrays _a and _d are allocated inside the function
   int32_t* _a;
   int32_t* _d;
	_a = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
	_d = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
  ///////////////////////////////////////////////////////////////////

   DBG_PRINT ("unnormalized on %zu@%p", lx, (void*) x);
      lx >>= K; // dividing lx by 2^K;

   for (k = K; k > 0UL; k--) {
      DBG_PRINT ("round %zu on %zu", k, K - 1U);
      /* Approximation part */
      // edge corrections
      // Haar transform
      x[0] = x[0] - long_mult_and_div (x[lx], 1L, 2L);
      _a[0] = x[0];

      if (lx > 1) {  
      // cdf(2,2) -- applied in all cases except when lx=1, i.e. there are only 2 points in the vector; this means that points 
      // a[0] and d[0] were calculated with the Haar transform only, which is applied before; also note, that x[1] should be 
      // overwritten with a new value because it is used later to inverse values of d vector; 
      x[1] = x[1] - long_mult_and_div (x[lx] + x[lx+1], 1L, 4L);
      _a[1] = x[1]; }

      for (n = 2UL; n < lx - 1UL; n ++) {
         m = n;
         x[m] += long_mult_and_div (x[lx+m-2UL] + x[lx+m+1UL], A, C);
         x[m] -= long_mult_and_div (x[lx+m-1UL] + x[lx+m], B, C);
         _a[n] = x[m];
      }

      if (lx > 2) {  
      // edge correction cdf(2,2) -- applied only when lx = 3 and more, i.e. it is skipped for the case when vector contains 
      // 4 points, a[0] a[1] d[0] d[1], i.e. lx =2; this is required because this condition becomes identical to condition for _a[1] above
      // when lx=2; in this case, x[1] becomes updated twice and therefore _a[1] = x[lx-1] -2*(x[(lx<<1)-1] + x[(lx<<1)-2])/ 4;
      x[lx-1] = x[lx-1] - long_mult_and_div (x[(lx<<1)-1] + x[(lx<<1)-2], 1L, 4L);
      _a[lx-1] = x[lx-1];}

      /* Detail part */
      for (n = 0UL; n < lx-1; n++) {
         m = n;
         x[m+lx] += (x[m] + x[m+1UL]) / 2L;
         _d[n] = x[m+lx];
      }
      // edge correction Haar transform
      x[(lx<<1) - 1] = x[(lx<<1) - 1] + x[lx - 1];
      _d[lx-1] = x[(lx<<1) - 1];

      /* Resort */
      for (n = 0; n < lx; n++) {
         x[(n<<1)] = _a[n];
         x[(n<<1)+1] = _d[n];
      }

      lx <<= 1; // after each scale undone, the length is increased by 2
   }
}

int icdf24 (int32_t* x, size_t lx, size_t K, int normalized) {
	// I don't know how to make this command work when arrays _a and _d are not defined globally at the beginning
	// of the file cdf24.c
   //DBG_PRINT ("temp buffers %jd @ %p and %p",
    //          (intmax_t)(CDF24_MAX_LENGTH >> 1), (void*)_a, (void*)_d);
    //
    //   this condition is no more needed since the size of the arrays is defined during execution of the code;
    //   this might not be good in the actual mermaid module; so this variant of the program should be only in the
    //   calculations on the computer;
//   if (lx > CDF24_MAX_LENGTH) {
//       DBG_PRINT ("length %zu exceed max allowed of %d", lx, CDF24_MAX_LENGTH);
//       return -1;
//   }
   if (lx % (1UL << K)) {
       DBG_PRINT ("length %zu is not a %ju multiple", lx, (1UL << K));
       return -1;
   }
// original block
//   if (normalized) normalized_cdf24 (x, lx, K);
//   else unnormalized_cdf24 (x, lx, K);
// my version of the block
	switch(normalized)
	{
		case 0 :
		unnormalized_cdf24(x,lx,K);
		break;
		case 1 :
		normalized_sqrt2_cdf24(x,lx,K);
		break;
		case 2 :
		normalized_2_cdf24(x,lx,K);
		break;
		default :
		normalized_2_cdf24(x,lx,K);
		break;
	}
   return 0;
}

