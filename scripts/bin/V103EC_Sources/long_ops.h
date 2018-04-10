/**************************************************************************//**
 * @file long_ops.h
 * @version 0.99
 * @date jeu. mai 14 00:37:45 CEST 2009
 *
 * @author jf.argentino@osean.fr
 * @brief 
 *
 * history: 
 *          -0.99: initial release
 ******************************************************************************/

#ifndef LONG_OPS_H
#define LONG_OPS_H

#include <stdint.h>

#if defined LONG_OPS_PARANOID && defined LONG_OPS_LAZY
   #error LONG_OPS_PARANOID and LONG_OPS_LAZY are mutual exclusive
#endif

/** @defgroup LONG_OPS Long integers operation
 *  @addtogroup LONG_OPS
 */
/*@{*/

/**************************************************************************//**
 * @brief A macro to help CPU instructions predictive branch.
 ******************************************************************************/
#define UNLIKELY(x) __builtin_expect((x),0)

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/**************************************************************************//**
 * @brief Overflow counter.
 ******************************************************************************/
extern uint32_t LONG_OPS_OVERFLOW;

/**************************************************************************//**
 * @brief Reset the overflow counter.
 ******************************************************************************/
void LONG_OPS_OVERFLOW_RESET (void);

/**************************************************************************//**
 * @brief Increment the overflow counter.
 ******************************************************************************/
void LONG_OPS_OVERFLOW_INCREMENT (void);

/**************************************************************************//**
 * @brief Test for overflow before adding two long intergers
 * @param[in] x The first long integer.
 * @param[in] y The second long integer.
 * @return 1 if adding x and y will overflow, 0 otherwise.
 ******************************************************************************/
int add_will_overflow (int32_t x, int32_t y);

/**************************************************************************//**
 * @brief Multiply two long integers and then divide the result by a third one.
 * @param[in] x The first long integer to multiply.
 * @param[in] num The second long integer to multiply.
 * @param[in] den The divisor, must be strictly greater than zero.
 * @return The result.
 ******************************************************************************/
int32_t long_mult_and_div (int32_t x, int32_t num, int32_t den);

/*@}*/
#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif

