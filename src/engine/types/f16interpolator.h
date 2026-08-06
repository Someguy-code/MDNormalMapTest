#ifndef _F16_INTERPOLATOR_H_
#define _F16_INTERPOLATOR_H_

#include <genesis.h>

typedef struct f16Interpolator
{
    f16 m_fInitialValue;
    f16 m_fTargetValue;
    f16 m_fBalance;
    f16 m_fSpeed;
    u16 m_uFramesLeft;
} f16Interpolator;

void InitializeF16Interpolator(f16Interpolator* _pF16Interpolator, f16 _fInitialValue, f16 _fTargetValue, u16 _uInterpolationFrames);
bool IsF16InterpolatorFinished(const f16Interpolator* _pF16Interpolator);
void FlipF16Interpolator(f16Interpolator* _pF16Interpolator, u16 _uInterpolationFrames);
f16 UpdateF16Interpolator(f16Interpolator* _pF16Interpolator);

#endif //#ifndef _F16_INTERPOLATOR_H_