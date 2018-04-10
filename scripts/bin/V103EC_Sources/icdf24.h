/**************************************************************************//**
 * @file cdf24.h
 * @version 0.99
 * @date jeu. mai 14 00:37:45 CEST 2009
 *
 * @author jf.argentino@osean.fr
 * @brief 
 *
 * history: 
 *          -0.99: initial release
 ******************************************************************************/

#ifndef ICDF24_H
#define ICDF24_H

#include <stdint.h>
#include <sys/types.h>

/** @defgroup CDF24 CDF[2,4] transform
 *  @addtogroup CDF24
 */
/*@{*/

/* For samplerate of 20Hz
#define CDF24_MAX_LENGTH 20000UL*/

/* For samplerate of 40Hz
#define CDF24_MAX_LENGTH 40000UL*/

/** The maximum length of a vector to transform, must be a 2^K multiple,
 *  the maximum signal duration is 1200s, so we have:
 *     -for sample rate of 20Hz, K = 5, CDF24_MAX_LENGTH = 24000...
 *     -for sample rate of 40Hz, K = 6, CDF24_MAX_LENGTH = 48000...
 *     -for sample rate of 110Hz, K = 8(?), CDF24_MAX_LENGTH = 132096
 */
#ifdef SAMPLE_RATE_20Hz
	enum { CDF24_MAX_LENGTH = 24032 };//original line
#else
	//enum { CDF24_MAX_LENGTH = 60672 };//my change of line
	enum { CDF24_MAX_LENGTH = 48064 };//original line
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************//**
 * @brief Perform \a K round of the
 * <tt>Cohen - Daubechies - Feauveau [2, 4]</tt> wavelet transform.
 *
 * The wavelet transform is made in-place. The vector length must be a multiple
 * of <tt>2^K</tt>, and not greater than MAX_LENGTH due to statically
 * allocated arrays used for the transformation.
 *
 * If signal \c x goes from \c 0 to <tt>lx - 1</tt>, wavelet coefficients after
 * \c K rounds are (exemple for <tt>K = 4</tt>):
 *
 * @verbatim
   lx/2^4     lx/2^3        lx/2^2                   lx/2^1
    <--> <--> <------> <--------------> <------------------------------>
   | A4 | D4 |   D3   |       D2       |               D1               |
   |    |    |        |                |                                |
   0    |  lx/2^3   lx/2^2           lx/2^1                             lx
      lx/2^4
   @endverbatim
 *
 * where \c A4  is the approximation part, and \c Dn (<tt>n</tt> from 1 to 4)
 * are the details.
 * 
 * The transformation can be normalized or not.
 *
 * @param[in,out] x The signal to transform and the result.
 * @param[in] lx The signal length.
 * @param[in] k The number of scales to perform.
 * @param[in] normalize Don't normalize the transform if 0.
 *
 * @return -1 if error, 0 otherwise.
 ******************************************************************************/
int icdf24 (int32_t* x, size_t lx, size_t k, int normalize)
   __attribute__((nonnull (1), warn_unused_result));

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

