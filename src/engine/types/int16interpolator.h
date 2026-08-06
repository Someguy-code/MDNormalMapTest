#ifndef _U16_INTERPOLATOR_H_
#define _U16_INTERPOLATOR_H_

#include <genesis.h>

typedef struct
{
    f16 m_fOffset;
    f16 m_fSpeed;
    u16 m_uFramesLeft;
} Int16InterpolatorBase;

typedef struct
{
    Int16InterpolatorBase m_oBase;
    u16 m_uTargetValue;
} u16Interpolator;

typedef struct
{
    Int16InterpolatorBase m_oBase;
    s16 m_sTargetValue;
} s16Interpolator;

void InitializeU16Interpolator(u16Interpolator* _pU16Interpolator, u16 _uInitialValue, u16 _uTargetValue, u16 _uInterpolationFrames);
bool IsU16InterpolatorInitialized(const u16Interpolator* _pU16Interpolator);
bool IsU16InterpolatorFinished(const u16Interpolator* _pU16Interpolator);
void FlipU16Interpolator(u16Interpolator* _pU16Interpolator, u16 _uTargetValue, u16 _uInterpolationFrames);
u16 UpdateU16Interpolator(u16Interpolator* _pU16Interpolator);

void InitializeS16Interpolator(s16Interpolator* _pS16Interpolator, s16 _sInitialValue, s16 _sTargetValue, u16 _uInterpolationFrames);
bool IsS16InterpolatorInitialized(const s16Interpolator* _pS16Interpolator);
bool IsS16InterpolatorFinished(const s16Interpolator* _pS16Interpolator);
void FlipS16Interpolator(s16Interpolator* _pS16Interpolator, s16 _sTargetValue, u16 _uInterpolationFrames);
s16 UpdateS16Interpolator(s16Interpolator* _pS16Interpolator);

#endif //#ifndef _U16_INTERPOLATOR_H_