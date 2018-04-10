/**************************************************************************//**
 * @file long_ops.c
 * @version 0.99
 * @date jeu. mai 14 00:37:45 CEST 2009
 *
 * @author jf.argentino@osean.fr
 * @brief 
 *
 * history: 
 *          -0.99: initial release
 ******************************************************************************/

#include "long_ops.h"

#ifdef FORCE_LONG_OPS_INLINING
#define __LONG_OPS_INLINE inline
#else
#define __LONG_OPS_INLINE
#endif

/******************************************************************************
 *
 ******************************************************************************/
uint32_t LONG_OPS_OVERFLOW = 0UL;
__LONG_OPS_INLINE void LONG_OPS_OVERFLOW_RESET (void) {LONG_OPS_OVERFLOW = 0UL;}
__LONG_OPS_INLINE void LONG_OPS_OVERFLOW_INCREMENT (void) {LONG_OPS_OVERFLOW++;}

/******************************************************************************
 *
 ******************************************************************************/
int add_will_overflow (int32_t x, int32_t y) {
   if ((x < 0L) && (y < 0L) && UNLIKELY (x <= INT32_MIN - y)) return 1;
   if ((x > 0L) && (y > 0L) && UNLIKELY (x >= INT32_MAX - y)) return 1;
   return 0;
}

#ifdef LONG_OPS_PARANOID
/******************************************************************************
 * abs ('num') must be greater than 'den', both are greater than zero
 ******************************************************************************/
static __LONG_OPS_INLINE int32_t long_neg_divide (int32_t num, int32_t den) {
   return (num / den) - ((-num % den) > (den / 2L) ? 1L : 0L);
}

/******************************************************************************
 *
 ******************************************************************************/
static __LONG_OPS_INLINE int32_t long_pos_divide (int32_t num, int32_t den) {
   return (num / den) + ((num % den) > (den / 2L) ? 1L : 0L);
}

/******************************************************************************
 *
 ******************************************************************************/
static __LONG_OPS_INLINE int32_t long_neg_mult_and_div (int32_t x,
                                                        int32_t num,
                                                        int32_t den) {
   int32_t rem;
   if (UNLIKELY(x <= (INT32_MIN / num))) {
      // Divide first but keep the remainder
      rem = -x % den;
      x /= den;
      if (UNLIKELY(x <= (INT32_MIN / num))) {
         LONG_OPS_OVERFLOW_INCREMENT ();
         return INT32_MIN;
      }
      return x * num - long_pos_divide (num * rem, den);
   }
   return long_neg_divide (x * num, den);
}

/******************************************************************************
 *
 ******************************************************************************/
static __LONG_OPS_INLINE int32_t long_pos_mult_and_div (int32_t x,
                                                        int32_t num,
                                                        int32_t den) {
   int32_t rem;
   if (UNLIKELY(x >= (INT32_MAX / num))) {
      // Divide first but keep the remainder
      rem = x % den;
      x /= den;
      if (UNLIKELY(x >= (INT32_MAX / num))) {
         LONG_OPS_OVERFLOW_INCREMENT ();
         return INT32_MAX;
      }
      return x * num + long_pos_divide (num * rem, den);
   }
   return long_pos_divide (x * num, den);
}

/******************************************************************************
 * Paranoid implementation
 ******************************************************************************/
__LONG_OPS_INLINE int32_t long_mult_and_div (int32_t x,
                                             int32_t num,
                                             int32_t den) {
   if (UNLIKELY(x == 0L)) return 0L;
   if (num < 0L) {
      num = -num;
      x = -x;
   }
   if (x < 0L) {
      return long_neg_mult_and_div (x, num, den);
   }
   return long_pos_mult_and_div (x, num, den);
}

#else /* not LONG_OPS_PARANOID */
#ifdef LONG_OPS_LAZY
/******************************************************************************
 * Lazy implementation, just do the basic work
 ******************************************************************************/
__LONG_OPS_INLINE int32_t long_mult_and_div (int32_t x,
                                             int32_t num,
                                             int32_t den) {
   return (x * num) / den;
}

#else
/******************************************************************************
 * Default implementation
 * TODO if (num * rem % den) > den / 2 add (or sub) 1 to the result 
 * Nb of ops (worst case): 3 comparisons, 4 affectations, 1 modulus,
 *                         2 multiplications, 2 divisions, 3 additions = 15
 ******************************************************************************/
__LONG_OPS_INLINE int32_t long_mult_and_div (int32_t x,
                                             int32_t num,
                                             int32_t den) {
   int32_t rem;
   /* 3 comparisons, 2 affectations, 1 modulus, 2 multiplications, 
      2 divisions, 1 addition */
   if (UNLIKELY(x == 0L)) return 0L;
   if (num < 0L) {
      /* 2 affectations, 2 additions */
      num = -num;
      x = -x;
   }
   if (x < 0L) {
      rem = -x % den;
      x /= den;
      return x * num - (num * rem) / den;
   }
   rem = x % den;
   x /= den;
   return x * num + (num * rem) / den;
}
#endif
#endif

