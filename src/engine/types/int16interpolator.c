#include "int16interpolator.h"

#include <engine/utils/mathutils.h>

static Int16InterpolatorBase CreateInt16Interpolator(f16 _fValueDelta, u16 _uInterpolationFrames);
static bool IsInt16InterpolatorInitialized(const Int16InterpolatorBase* _pInt16Interpolator);
static bool IsInt16InterpolatorFinished(const Int16InterpolatorBase* _pInt16Interpolator);
static void FlipInt16Interpolator(Int16InterpolatorBase* _pInt16Interpolator, f16 _fValueDelta, u16 _uInterpolationFrames);
static s16 UpdateInt16Interpolator(Int16InterpolatorBase* _pInt16Interpolator);

void InitializeU16Interpolator(u16Interpolator* _pU16Interpolator, u16 _uInitialValue, u16 _uTargetValue, u16 _uInterpolationFrames)
{
    const f16 fValueDelta = FIX16(_uTargetValue - _uInitialValue);
    *_pU16Interpolator =  (u16Interpolator) {
        .m_oBase = CreateInt16Interpolator(fValueDelta, _uInterpolationFrames),
        .m_uTargetValue = _uTargetValue,
    };
}

bool IsU16InterpolatorInitialized(const u16Interpolator* _pU16Interpolator)
{
    return IsInt16InterpolatorInitialized(&_pU16Interpolator->m_oBase);
}

bool IsU16InterpolatorFinished(const u16Interpolator* _pU16Interpolator)
{
    return IsInt16InterpolatorFinished(&_pU16Interpolator->m_oBase);
}

void FlipU16Interpolator(u16Interpolator* _pU16Interpolator, u16 _uTargetValue, u16 _uInterpolationFrames)
{
    const f16 fValueDelta = FIX16(_uTargetValue - _pU16Interpolator->m_uTargetValue);
    FlipInt16Interpolator(&_pU16Interpolator->m_oBase, fValueDelta,  _uInterpolationFrames);
    _pU16Interpolator->m_uTargetValue = _uTargetValue;
}

u16 UpdateU16Interpolator(u16Interpolator* _pU16Interpolator)
{
    const s16 sOffset = UpdateInt16Interpolator(&_pU16Interpolator->m_oBase);
    return _pU16Interpolator->m_uTargetValue - sOffset;
}

void InitializeS16Interpolator(s16Interpolator* _pS16Interpolator, s16 _sInitialValue, s16 _sTargetValue, u16 _uInterpolationFrames)
{
    const f16 fValueDelta = FIX16(_sTargetValue - _sInitialValue);
    *_pS16Interpolator =  (s16Interpolator) {
        .m_oBase = CreateInt16Interpolator(fValueDelta, _uInterpolationFrames),
        .m_sTargetValue = _sTargetValue,
    };
}

bool IsS16InterpolatorInitialized(const s16Interpolator* _pS16Interpolator)
{
    return IsInt16InterpolatorInitialized(&_pS16Interpolator->m_oBase);
}

bool IsS16InterpolatorFinished(const s16Interpolator* _pS16Interpolator)
{
    return IsInt16InterpolatorFinished(&_pS16Interpolator->m_oBase);
}

void FlipS16Interpolator(s16Interpolator* _pS16Interpolator, s16 _sTargetValue, u16 _uInterpolationFrames)
{
    const f16 fValueDelta = FIX16(_sTargetValue - _pS16Interpolator->m_sTargetValue);
    FlipInt16Interpolator(&_pS16Interpolator->m_oBase, fValueDelta,  _uInterpolationFrames);
    _pS16Interpolator->m_sTargetValue = _sTargetValue;
}

s16 UpdateS16Interpolator(s16Interpolator* _pS16Interpolator)
{
    const s16 sOffset = UpdateInt16Interpolator(&_pS16Interpolator->m_oBase);
    return _pS16Interpolator->m_sTargetValue - sOffset;
}

static Int16InterpolatorBase CreateInt16Interpolator(f16 _fValueDelta, u16 _uInterpolationFrames)
{
    return (Int16InterpolatorBase){
        .m_fOffset = _fValueDelta,
        .m_fSpeed = _uInterpolationFrames == 0 ? 0 : F16_div(_fValueDelta, FIX16(_uInterpolationFrames)),
        .m_uFramesLeft = _uInterpolationFrames
    };
}

static bool IsInt16InterpolatorInitialized(const Int16InterpolatorBase* _pInt16Interpolator)
{
    return _pInt16Interpolator->m_fSpeed != 0;
}

static bool IsInt16InterpolatorFinished(const Int16InterpolatorBase* _pInt16Interpolator)
{
    return _pInt16Interpolator->m_uFramesLeft == 0;
}

static void FlipInt16Interpolator(Int16InterpolatorBase* _pInt16Interpolator, f16 _fValueDelta, u16 _uInterpolationFrames)
{
    _pInt16Interpolator->m_fOffset = _fValueDelta - COPY_SIGN(_fValueDelta, _pInt16Interpolator->m_fOffset);
    _pInt16Interpolator->m_fSpeed = -_pInt16Interpolator->m_fSpeed;
    _pInt16Interpolator->m_uFramesLeft = _uInterpolationFrames - _pInt16Interpolator->m_uFramesLeft;
}

static s16 UpdateInt16Interpolator(Int16InterpolatorBase* _pInt16Interpolator)
{
    s16 sOffset = 0;
    u16 uFramesLeft = _pInt16Interpolator->m_uFramesLeft;
    if(uFramesLeft > 0)
    {
        --uFramesLeft;
        if(uFramesLeft > 0)
        {
            f16* pOffset = &_pInt16Interpolator->m_fOffset;
            *pOffset -= _pInt16Interpolator->m_fSpeed;
            sOffset = F16_toRoundedInt(*pOffset);
        }
        _pInt16Interpolator->m_uFramesLeft = uFramesLeft;
    }

    return sOffset;
}
