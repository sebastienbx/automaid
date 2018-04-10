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

#include "cdf24.h"
#include "long_ops.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef DBG_CDF24
   #ifdef MERMAID_ECOS_BUILD
   #include "../Message.h"
   #define DBG_PRINT(x, ...) write_DBG("CDF24", __LINE__, x , ##__VA_ARGS__);
   #else /* MERMAID_ECOS_BUILD not defined */
   #define DBG_PRINT(x, ...) printf("CDF24 %d: " x "\n", __LINE__,\
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
   size_t k, n, m;
   // my addition - arrays _a and _d are allocated inside the function
   int32_t* _a;
   int32_t* _d;
	_a = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
	_d = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
  ///////////////////////////////////////////////////////////////////

   DBG_PRINT ("normalized on %zu@%p", lx, (void*) x);
   for (k = 0UL; k < K; k++) {
      DBG_PRINT ("round %zu on %zu", k, K - 1U);
      lx >>= 1;
      /* Detail part */
      for (n = 1UL; n < lx; n++) {
         m = (n << 1) - 1UL;
         x[m] -= (x[m-1UL] + x[m+1UL]) / 2L;
         _d[n-1] = long_mult_and_div (x[m], SQRT2_DEN, SQRT2_NUM);
      }
      _d[lx-1] = long_mult_and_div (x[(lx<<1) - 1], SQRT2_DEN, SQRT2_NUM);
      /* Approximation part */
      _a[0] = long_mult_and_div (x[0], SQRT2_NUM, SQRT2_DEN);
      _a[1] = long_mult_and_div (x[2], SQRT2_NUM, SQRT2_DEN);
      for (n = 2UL; n < lx - 1UL; n ++) {
         m = n << 1;
         x[m] -= long_mult_and_div (x[m-3UL] + x[m+3UL], A, C);
         x[m] += long_mult_and_div (x[m-1UL] + x[m+1UL], B, C);
         _a[n] = long_mult_and_div (x[m], SQRT2_NUM, SQRT2_DEN);
      }
      _a[lx-2] = long_mult_and_div (x[(lx<<1) - 4], SQRT2_NUM, SQRT2_DEN);
      _a[lx-1] = long_mult_and_div (x[(lx<<1) - 2], SQRT2_NUM, SQRT2_DEN);
      /* Resort */
      for (n = 0; n < lx; n++) {
         x[n] = _a[n];
         x[lx + n] = _d[n];
      }
   }
}


static void normalized_2_cdf24 (int32_t* x, size_t lx, size_t K) {
   size_t k, n, m;
   // my addition - arrays _a and _d are allocated inside the function
   int32_t* _a;
   int32_t* _d;
	_a = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
	_d = (int32_t *) malloc(sizeof(int32_t)*lx >> 1);
  ///////////////////////////////////////////////////////////////////

   DBG_PRINT ("normalized on %zu@%p", lx, (void*) x);
   for (k = 0UL; k < K; k++) {
      DBG_PRINT ("round %zu on %zu", k, K - 1U);
//	if ( (k+1)%2)
//	{printf("odd scale 2^%ld\n",(k+1));}
//	else
//	{printf("even scale 2^%ld\n",(k+1));}
      lx >>= 1;
      /* Detail part */
      for (n = 1UL; n < lx; n++) {
         m = (n << 1) - 1UL;
         x[m] -= (x[m-1UL] + x[m+1UL]) / 2L;
//my addition of if block
	if ((k+1)%2)
          {_d[n-1] = long_mult_and_div (x[m], SQRT2_DEN, SQRT2_NUM);}
	else
          {_d[n-1] = x[m];}
///////////////////////
         //_d[n-1] = long_mult_and_div (x[m], SQRT2_DEN, SQRT2_NUM);//original line
      }
	if ((k+1)%2)
        {_d[lx-1] = long_mult_and_div (x[(lx<<1) - 1], SQRT2_DEN, SQRT2_NUM);}
	else
        {_d[lx-1] = x[(lx<<1) - 1];}
      /* Approximation part */
//my addition of if block - normalization of a is done for even scale by multiplying by 2
	if ( ~(k+1)%2)
        {//_a[0] = long_mult_and_div (x[0],2L,1L);
         //_a[1] = long_mult_and_div (x[2],2L,1L);}
	  _a[0] = x[0] << 1; _a[1] = x[2] << 1;}
        else
        {_a[0] = x[0];
         _a[1] = x[2];}
      for (n = 2UL; n < lx - 1UL; n ++) {
         m = n << 1;
         x[m] -= long_mult_and_div (x[m-3UL] + x[m+3UL], A, C);
         x[m] += long_mult_and_div (x[m-1UL] + x[m+1UL], B, C);
	if ( ~(k+1)%2)
        {//_a[n] = x[m];_a[n] <<= 1;}
        _a[n] = x[m]<<1;}
	else
        {_a[n] = x[m];}
      }
	if (~(k+1)%2)
        {//_a[lx-2] = long_mult_and_div (x[(lx<<1) - 4],2L,1L);
         //_a[lx-1] = long_mult_and_div (x[(lx<<1) - 2],2L,1L);}
         _a[lx-1] = x[(lx<<1) - 2] << 1;}
        else
        {//_a[lx-2] = x[(lx<<1) - 4];
         _a[lx-1] = x[(lx<<1) - 2];}
      /* Resort */
      for (n = 0; n < lx; n++)
	 {x[n] = _a[n];
         x[lx + n] = _d[n];}
   }// loop over k
      for (n = 0; n < lx; n++)
	 {
	if (k % 2)
	 { x[n] = long_mult_and_div (_a[n], SQRT2_NUM, SQRT2_DEN);}
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
   for (k = 0UL; k < K; k++) {
      DBG_PRINT ("round %zu on %zu", k, K - 1U);
      lx >>= 1;
      /* Detail part */
      for (n = 1UL; n < lx; n++) {
         m = (n << 1) - 1UL;
         x[m] -= (x[m-1UL] + x[m+1UL]) / 2L;
         _d[n-1] = x[m];
      }
      _d[lx-1] = x[(lx<<1) - 1];
      /* Approximation part */
      _a[0] = x[0];
      _a[1] = x[2];
      for (n = 2UL; n < lx - 1UL; n ++) {
         m = n << 1;
         x[m] -= long_mult_and_div (x[m-3UL] + x[m+3UL], A, C);
         x[m] += long_mult_and_div (x[m-1UL] + x[m+1UL], B, C);
         _a[n] = x[m];
      }
      _a[lx-2] = x[(lx<<1) - 4];
      _a[lx-1] = x[(lx<<1) - 2];
      /* Resort */
      for (n = 0; n < lx; n++) {
         x[n] = _a[n];
         x[lx + n] = _d[n];
      }
   }
}


int cdf24 (int32_t* x, size_t lx, size_t K, int normalized) {
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

