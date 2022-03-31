/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: FFT.h
 *
 * Code generated for Simulink model 'FFT'.
 *
 * Model version                  : 1.44
 * Simulink Coder version         : 9.1 (R2019a) 23-Nov-2018
 * C/C++ source code generated on : Fri Mar  6 05:13:11 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Texas Instruments->C5000
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_FFT_h_
#define RTW_HEADER_FFT_h_
#include <math.h>
#include <string.h>
#include <stddef.h>
#ifndef FFT_COMMON_INCLUDES_
# define FFT_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* FFT_COMMON_INCLUDES_ */

#include "FFT_types.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Constant parameters (default storage) */
typedef struct {
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<Root>/FFT'
   */
  real32_T FFT_TwiddleTable[192];
} ConstP_FFT_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  int16_T In1[256];                    /* '<Root>/In1' */
} ExtU_FFT_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  int16_T Out1[256];                   /* '<Root>/Out1' */
} ExtY_FFT_T;

/* Real-time Model Data Structure */
struct tag_RTM_FFT_T {
  const char_T * volatile errorStatus;
};

/* External inputs (root inport signals with default storage) */
extern ExtU_FFT_T FFT_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_FFT_T FFT_Y;

/* Constant parameters (default storage) */
extern const ConstP_FFT_T FFT_ConstP;

/* Model entry point functions */
extern void FFT_initialize(void);
extern void FFT_step(void);
extern void FFT_terminate(void);

/* Real-time Model object */
extern RT_MODEL_FFT_T *const FFT_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'FFT'
 */
#endif                                 /* RTW_HEADER_FFT_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
