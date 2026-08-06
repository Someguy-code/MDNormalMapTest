#include "mathutils.h"

//Returns the value of the sinus from the cossiun (or viceversa). _fCos must be in the [1, -1] range.
FORCE_INLINE f16 GetSinFromCos(f16 _fCos)
{
    return F16_sqrt(FIX16(1) - F16_mul(_fCos, _fCos));
}

FORCE_INLINE f16 GetAngleFromCos(f16 _fCos)
{
    return F16_atan2(GetSinFromCos(_fCos), _fCos);
}

FORCE_INLINE u16 GetIntegerRadiansFromDegrees(f16 _fAngle)
{
    return (F16_toRoundedInt(_fAngle) << 10) / 360;
}

FORCE_INLINE u16 GetAngleInIntegerRadians(s16 _sX, s16 _sY)
{
    return GetIntegerRadiansFromDegrees(F16_atan2(FIX16(_sY), FIX16(_sX)));
}

//Must return a 32bit integer because the result is in the [49152, -48768]
FORCE_INLINE s32 DotProduct(const Vector3_s8* _pVector1, const Vector3_s8* _pVector2)
{
    return _pVector1->m_sX * _pVector2->m_sX + _pVector1->m_sY * _pVector2->m_sY + _pVector1->m_sZ * _pVector2->m_sZ;
}