#ifndef __nds32_CONTROLLER_MATH_H__
#define __nds32_CONTROLLER_MATH_H__
/***************************************************************************
 * Copyright (C) 2012-2018 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup controller Controller Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"


// simulation function for dsp inst.
static inline q31_t perf_test_kaddw(q31_t x, q31_t y)
{
    q31_t ret = x + y;
#ifdef __riscv
    if(perf_test_sva(x, y) != 0)
#elif __NDS32__
    if ((__nds32__sva(x, y) != 0) )
#endif
    {
        ret = (x >> 31) ^ 0x7FFFFFFF;
    }
    return ret;
}

static inline q31_t perf_test_ksubw(q31_t x, q31_t y)
{
    q31_t ret = x - y;
 #ifdef __riscv
    if(perf_test_svs(x, y) != 0)
#elif __NDS32__
    if ((__nds32__svs(x, y) != 0) )
#endif
    {
        ret = (x >> 31) ^ 0x7FFFFFFF;
    }
    return ret;
}

// Clarke Transform
/**
 * @brief Clarke transform of floating-point input.
 * @param[in]       a       input three-phase coordinate a.
 * @param[in]       b       input three-phase coordinate b.
 * @param[out]      *alpha  output two-phase orthogonal vector axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vector axis beta.
 * @return none.
 */
static inline void nds32_clarke_f32(float32_t a, float32_t b, float32_t *alpha,
                      float32_t *beta)
{
#ifndef ENA_EMPTY_FUNC
    *alpha = a;
    /* beta = ((1 / sqrt(3))* a + (2 / sqrt(3)) * b */
    *beta = ((float32_t) 0.57735026919 * a + (float32_t) 1.15470053838 * b);
#else
    nds32_empty_func4(a, b, alpha, beta);
#endif
}
/**
 * @brief Clarke transform of q31 input.
 * @param[in]       a       input three-phase coordinate a.
 * @param[in]       b       input three-phase coordinate b.
 * @param[out]      *alpha  output two-phase orthogonal vector axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vector axis beta.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void nds32_clarke_q31(q31_t a, q31_t b, q31_t *alpha, q31_t *beta)
{
#ifndef ENA_EMPTY_FUNC

    const q31_t recip_1_sqrt3 = (q31_t)0x24F34E8B;
    const q31_t recip_2_sqrt3 = (q31_t)0x49E69D16;

#ifdef __NDS32_EXT_DSP__
    q63_t tmp1;
    q31_t tmp2;

    tmp1 = (q63_t)a * recip_1_sqrt3;
    tmp1 = __nds32__kmar64(tmp1, b, recip_2_sqrt3);
    tmp2 = (q31_t)(tmp1 >> 30);     //has overflow risk

#else   //__NDS32_EXT_DSP__

    q31_t tmp1, tmp2;

    tmp1 = (q31_t) (((q63_t) a * recip_1_sqrt3) >> 30);
    tmp2 = (q31_t) (((q63_t) b * recip_2_sqrt3) >> 30);

#ifdef ENA_EXT_SAT_INST
    tmp2 = (q31_t) __nds32__kaddw(tmp1, tmp2);
#else
    tmp2 = (q31_t) perf_test_kaddw(tmp1, tmp2);
#endif

#endif  //__NDS32_EXT_DSP__

    *alpha = a;
    *beta = tmp2;

#else   //ENA_EMPTY_FUNC
    nds32_empty_func4(a, b, alpha, beta);
#endif
}

// Inverse Clarke Transform
/**
 * @brief Inverse Clarke transform of floating-point input.
 * @param[in]       alpha   input two-phase orthogonal vector axis alpha.
 * @param[in]       beta    input two-phase orthogonal vector axis beta.
 * @param[out]      *a      output three-phase coordinate a.
 * @param[in]       *b      output three-phase coordinate b.
 * @return none.
 */
static inline void nds32_inv_clarke_f32(float32_t alpha, float32_t beta, float32_t *a,
                                        float32_t *b)
{
#ifndef ENA_EMPTY_FUNC
    *a = alpha;
    /* b = -0.5 * alpha + (sqrt(3) / 2) * beta */
    *b = (float32_t) -0.5 * alpha + (float32_t) 0.8660254039 * beta;
#else
    nds32_empty_func4(alpha, beta, a, b);
#endif
}
/**
 * @brief Inverse Clarke transform of q31 input.
 * @param[in]       alpha   input two-phase orthogonal vector axis alpha.
 * @param[in]       beta    input two-phase orthogonal vector axis beta.
 * @param[out]      *a      output three-phase coordinate a.
 * @param[in]       *b      output three-phase coordinate b.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void nds32_inv_clarke_q31(q31_t alpha, q31_t beta, q31_t *a, q31_t *b)
{
#ifndef ENA_EMPTY_FUNC

    q31_t tmp1, tmp2;
    const q31_t half = (q31_t)0x40000000;
    const q31_t sqrt3_2 = (q31_t)0x6ED9EBA1;

#ifdef __NDS32_EXT_DSP__

    //no overflow risk.
    tmp1 = (q31_t)__nds32__kwmmul(alpha, half);
    tmp2 = (q31_t)__nds32__kwmmul(beta, sqrt3_2);
    tmp2 = (q31_t)__nds32__ksubw(tmp2, tmp1);

#else   //__NDS32_EXT_DSP__

    tmp1 = (q31_t) (((q63_t)alpha * half) >> 31);
    tmp2 = (q31_t) (((q63_t)beta * sqrt3_2) >> 31);

#ifdef ENA_EXT_SAT_INST
    tmp2 = (q31_t) __nds32__ksubw(tmp2, tmp1);
#else
    tmp2 = (q31_t) perf_test_ksubw(tmp2, tmp1);
#endif

#endif  //__NDS32_EXT_DSP__

    *a = alpha;
    *b = tmp2;

#else   //ENA_EMPTY_FUNC
    nds32_empty_func4(alpha, beta, a, b);
#endif
}

// Park Transform
/**
 * @brief Park transform of floating-point input.
 * @param[in]       alpha input two-phase coordinate alpha.
 * @param[in]       beta  input two-phase coordinate beta.
 * @param[out]      *a    output rotor frame a.
 * @param[out]      *b    output rotor frame b.
 * @param[in]       sin   sine value of rotation angle θ.
 * @param[in]       cos   cosine value of rotation angle θ.
 * @return none.
 */
static inline void nds32_park_f32(float32_t alpha, float32_t beta, float32_t *a,
                                  float32_t *b, float32_t sin, float32_t cos)
{
#ifndef ENA_EMPTY_FUNC
    *a = alpha * cos + beta * sin;
    *b = -alpha * sin + beta * cos;
#else
    nds32_empty_func6(alpha, beta, a, b, sin, cos);
#endif
}

/**
 * @brief Park transform of q31 input.
 * @param[in]       alpha input two-phase coordinate alpha.
 * @param[in]       beta  input two-phase coordinate beta.
 * @param[out]      *a    output rotor frame a.
 * @param[out]      *b    output rotor frame b.
 * @param[in]       sin   sine value of rotation angle θ.
 * @param[in]       cos   cosine value of rotation angle θ.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void nds32_park_q31(q31_t alpha, q31_t beta, q31_t *a, q31_t *b,
                                  q31_t sin, q31_t cos)
{
#ifndef ENA_EMPTY_FUNC
    q31_t tmp1, tmp2, tmp3, tmp4;

    tmp1 = (q31_t) (((q63_t) (alpha) * (cos)) >> 31);
    tmp2 = (q31_t) (((q63_t) (beta) * (sin)) >> 31);
    tmp3 = (q31_t) (((q63_t) (alpha) * (sin)) >> 31);
    tmp4 = (q31_t) (((q63_t) (beta) * (cos)) >> 31);

#if defined(__NDS32_EXT_DSP__) || defined(ENA_EXT_SAT_INST)
    *a = (q31_t) __nds32__kaddw(tmp1, tmp2);
    *b = (q31_t) __nds32__ksubw(tmp4, tmp3);
#else
    *a = (q31_t) perf_test_kaddw(tmp1, tmp2);
    *b = (q31_t) perf_test_ksubw(tmp4, tmp3);
#endif

#else
    nds32_empty_func6(alpha, beta, a, b, sin, cos);
#endif
}
// Inverse Park Transform
/**
 * @brief Inverse Park transform of floating-point input.
 * @param[in]       a       input coordinate of rotor frame a.
 * @param[in]       b       input coordinate of rotor frame b.
 * @param[out]      *alpha  output two-phase orthogonal vec axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vec axis beta.
 * @param[in]       sin     sine value of rotation angle θ.
 * @param[in]       cos     cosine value of rotation angle θ.
 * @return none.
 */
static inline void nds32_inv_park_f32(float32_t a, float32_t b, float32_t *alpha,
                                      float32_t *beta, float32_t sin, float32_t cos)
{
#ifndef ENA_EMPTY_FUNC
    *alpha = a * cos - b * sin;
    *beta = a * sin + b * cos;
#else
    nds32_empty_func6(a, b, alpha, beta, sin, cos);
#endif
}
/**
 * @brief Inverse Park transform of q31 input.
 * @param[in]       a       input coordinate of rotor frame a.
 * @param[in]       b       input coordinate of rotor frame b.
 * @param[out]      *alpha  output two-phase orthogonal vec axis alpha.
 * @param[out]      *beta   output two-phase orthogonal vec axis beta.
 * @param[in]       sin     sine value of rotation angle θ.
 * @param[in]       cos     cosine value of rotation angle θ.
 * @return none.
 *
 * The internal 32-bit accumulator maintains 1.31 format by truncating lower
 * 31 bits of the intermediate multiplication in 2.62 format.
 */
static inline void nds32_inv_park_q31(q31_t a, q31_t b, q31_t *alpha, q31_t *beta,
                                      q31_t sin, q31_t cos)
{
#ifndef ENA_EMPTY_FUNC
    q31_t tmp1, tmp2, tmp3, tmp4;

    tmp1 = (q31_t) (((q63_t) (a) * (cos)) >> 31);
    tmp2 = (q31_t) (((q63_t) (b) * (sin)) >> 31);
    tmp3 = (q31_t) (((q63_t) (a) * (sin)) >> 31);
    tmp4 = (q31_t) (((q63_t) (b) * (cos)) >> 31);

#if defined(__NDS32_EXT_DSP__) || defined(ENA_EXT_SAT_INST)
    *alpha = (q31_t) __nds32__ksubw(tmp1, tmp2);
    *beta = (q31_t) __nds32__kaddw(tmp4, tmp3);
#else
    *alpha = (q31_t) perf_test_ksubw(tmp1, tmp2);
    *beta = (q31_t) perf_test_kaddw(tmp4, tmp3);
#endif
#else
    nds32_empty_func6(a, b, alpha, beta, sin, cos);
#endif
}

// PID Control
/**
 * @brief Structure for the floating-point PID Control.
 */
typedef struct
{
    float32_t gain1;       // gain1 = Kp + Ki + Kd.
    float32_t gain2;       // gain2 = -Kp - 2Kd.
    float32_t gain3;       // gain3 = Kd.
    float32_t state[3];    // State buffer of length 3.
    float32_t Kp;          // Proportional gain, a tuning parameter.
    float32_t Ki;          // Integral gain, a tuning parameter.
    float32_t Kd;          // Derivative gain, a tuning parameter.
} nds32_pid_f32_t;

/**
 * @brief PID control of floating-point input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
static inline
float32_t nds32_pid_f32(nds32_pid_f32_t *instance, float32_t src)
{
#ifndef ENA_EMPTY_FUNC
    float32_t dst;

    /* dst[k] = dst[k - 1] + gain1 * src[k] + gain2 * src[k - 1] */
    dst = (instance->gain1 * src) + (instance->gain2 * instance->state[0])
          + (instance->gain3 * instance->state[1]) + (instance->state[2]);

    instance->state[1] = instance->state[0];
    instance->state[0] = src;
    instance->state[2] = dst;

    return dst;
#else
    nds32_empty_ret_func2(instance, src);
#endif
}
/**
 * @brief PID initializatopn control function of floating-point formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
void nds32_init_pid_f32(nds32_pid_f32_t *instance, int32_t set);

/**
 * @brief Structure for the Q31 PID Control.
 */
typedef struct
{
    q31_t gain1;       // gain1 = Kp + Ki + Kd.
    q31_t gain2;       // gain2 = -Kp - 2Kd.
    q31_t gain3;       // gain3 = Kd.
    q31_t state[3];    // State buffer of length 3.
    q31_t Kp;          // Proportional gain, a tuning parameter.
    q31_t Ki;          // Integral gain, a tuning parameter.
    q31_t Kd;          // Derivative gain, a tuning parameter.
} nds32_pid_q31_t;

/**
 * @brief PID control of Q31 input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
static inline q31_t nds32_pid_q31(nds32_pid_q31_t *instance, q31_t src)
{
#ifndef ENA_EMPTY_FUNC
    q63_t tmp;
    q31_t dst;

    /* dst[k] = dst[k - 1] + gain1 * src[k] + gain2 * src[k - 1] */
    tmp = (q63_t) instance->gain1 * src;
    tmp += (q63_t) instance->gain2 * instance->state[0];
    tmp += (q63_t) instance->gain3 * instance->state[1];

    dst = (q31_t) (tmp >> 31u);
    dst += instance->state[2];


    instance->state[1] = instance->state[0];
    instance->state[0] = src;
    instance->state[2] = dst;

    return dst;
#else
    nds32_empty_ret_func2(instance, src);
#endif
}

/**
 * @brief PID initializatopn control function of Q31 formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
void nds32_init_pid_q31(nds32_pid_q31_t *instance, int32_t set);

/**
 * @brief Structure for the Q15 PID Control.
 */
typedef struct
{
    q15_t gain1;       // gain1 = Kp + Ki + Kd.
    q15_t gain2;       // gain2 = -Kp - 2Kd.
    q15_t gain3;       // gain3 = Kd.
    q15_t state[3];    // State buffer of length 3.
    q15_t Kp;          // Proportional gain, a tuning parameter.
    q15_t Ki;          // Integral gain, a tuning parameter.
    q15_t Kd;          // Derivative gain, a tuning parameter.
} nds32_pid_q15_t;

/**
 * @brief PID control of Q15 input.
 * @param[in, out]    *instance     points to an instance of the PID
 *                                  controliler.
 * @param[in] src    input data.
 * @return output data.
 */
#if defined(__NDS32_EXT_DSP__) || (__NDS32_EXT_PERF__ == 1)

#define perf_test_sats(X, Y)        __nds32__clips(X, (Y - 1))

#else
static inline q31_t perf_test_sats(q31_t src, const uint32_t bit_size)
{
    q31_t ret;

    const q31_t max_val = (q31_t)0x7FFFFFFF >> (32 - bit_size);
    const q31_t min_val = (q31_t)0x80000000 >> (32 - bit_size);
    ret = (src > max_val) ? max_val : (src < min_val) ? min_val : src;

    return ret;
}
#endif

#define LOCAL_SCALE_DOWN    8

static inline q15_t nds32_pid_q15(nds32_pid_q15_t *instance, q15_t src)
{
#ifndef ENA_EMPTY_FUNC
    q31_t ret;

    ret = ((q31_t)instance->gain1 * src) >> LOCAL_SCALE_DOWN;
    ret += ((q31_t)instance->gain2 * instance->state[0]) >> LOCAL_SCALE_DOWN;
    ret += ((q31_t) instance->gain3 * instance->state[1]) >> LOCAL_SCALE_DOWN;
    ret += ((q31_t) instance->state[2] << (15 - LOCAL_SCALE_DOWN));

    ret = perf_test_sats(ret >> (15 - LOCAL_SCALE_DOWN), 16u);

    instance->state[1] = instance->state[0];
    instance->state[0] = src;
    instance->state[2] = (q15_t)ret;

    return (q15_t)ret;

#else
    nds32_empty_ret_func2(instance, src);
#endif
}
/**
 * @brief PID initializatopn control function of Q15 formats.
 * @param[in, out]    *instance    points to an instance of the PID
 *                                 controliler.
 * @param[in]               set    for 1 will clear the state to all zeros
 *                                     0 will not.
 * @return none.
 *
 * This function will calculate the PID control structure gain
 * <code>gain1</code>, <code>gain2</code> and <code>gain3</code> by seting
 * the variable <code>Kp</code>, <code>Ki</code> and <code>Kd</code>. The
 * state variable will set to all zeros.
 */
void nds32_init_pid_q15(nds32_pid_q15_t *instance, int32_t set);

#ifdef  __cplusplus
}
#endif
#endif // __NDS32_TRANSFORM_MATH_H__
