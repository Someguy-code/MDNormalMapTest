#include "lightutils.h"

#include <engine/utils/mathutils.h>

Vect3D_f16 GetLightDirection(s16 _sDeltaCenterX, s16 _sDeltaCenterY, u16 _uMaxDistance)
{   
    //Compute vertical component of the light as the distance to the center and map it into the [0, 255] range
    const u32 uDistanceToCenter = getApproximatedDistance(_sDeltaCenterX, _sDeltaCenterY) << 8;
    u16 uHeight = uDistanceToCenter / _uMaxDistance;
    uHeight = 255 - clamp(uHeight, 0, 255);
    const f16 LightZ = -sinFix16(uHeight);
    const f16 LightHorizontalMul = -cosFix16(uHeight);

    //Compute horizontal angle (in degrees) and map it into the [0, 1024] range
    const u16 uHorizontalAngle = GetAngleInIntegerRadians(_sDeltaCenterX, -_sDeltaCenterY);

    return (Vect3D_f16){
        F16_mul(cosFix16(uHorizontalAngle), LightHorizontalMul),
        F16_mul(sinFix16(uHorizontalAngle), LightHorizontalMul),
        LightZ
    };
}

Vector3_s8 GetLightDirectionS8(const Vect3D_f16* _pLightDirection)
{
    const f16 fMaxValue = FIX16(127);
    return (Vector3_s8){
        .m_sX = F16_toRoundedInt(F16_mul(_pLightDirection->x, fMaxValue)),
        .m_sY = F16_toRoundedInt(F16_mul(_pLightDirection->y, fMaxValue)),
        .m_sZ = F16_toRoundedInt(F16_mul(_pLightDirection->z, fMaxValue)),
    };
}