#include "f16interpolator.h"

#include <engine/utils/mathutils.h>

//Multiplier used to increase the range of the balance (so m_fSpeed doesn't become 0 so easily). The resolution of the interpolation is still limited to 64 levels, though
static const s16 MAX_BALANCE = 256;

void InitializeF16Interpolator(f16Interpolator* _pF16Interpolator, f16 _fInitialValue, f16 _fTargetValue, u16 _uInterpolationFrames)
{
    *_pF16Interpolator =  (f16Interpolator) {
        .m_fInitialValue = _fInitialValue, 
        .m_fTargetValue = _fTargetValue,
        .m_fBalance = 0,
        .m_fSpeed = F16_div(FIX16(MAX_BALANCE), FIX16(_uInterpolationFrames)),
        .m_uFramesLeft = _uInterpolationFrames
    };
}

bool IsF16InterpolatorFinished(const f16Interpolator* _pF16Interpolator)
{
    return _pF16Interpolator->m_uFramesLeft == 0;
}

void FlipF16Interpolator(f16Interpolator* _pF16Interpolator, u16 _uInterpolationFrames)
{
    SWAP_s16(_pF16Interpolator->m_fTargetValue, _pF16Interpolator->m_fInitialValue);
    _pF16Interpolator->m_fBalance = MAX_BALANCE - _pF16Interpolator->m_fBalance;
    _pF16Interpolator->m_uFramesLeft = _uInterpolationFrames - _pF16Interpolator->m_uFramesLeft;
}

f16 UpdateF16Interpolator(f16Interpolator* _pF16Interpolator)
{
    f16 fBalance = FIX16(1);
    u16 uFramesLeft = _pF16Interpolator->m_uFramesLeft;
    if(uFramesLeft > 0)
    {
        --uFramesLeft;
        if(uFramesLeft > 0)
        {
            fBalance = _pF16Interpolator->m_fBalance + _pF16Interpolator->m_fSpeed;
            _pF16Interpolator->m_fBalance = fBalance;
            fBalance /= MAX_BALANCE;
        }
        _pF16Interpolator->m_uFramesLeft = uFramesLeft;
    }

    return LERP_F16(_pF16Interpolator->m_fInitialValue, _pF16Interpolator->m_fTargetValue, fBalance);
}
