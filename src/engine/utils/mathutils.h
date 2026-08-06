#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include <genesis.h>

#include <engine/utils/macroutils.h>
#include <engine/types/vector3.h>

#define IS_2D_VECTOR_NULL(oVector) ({ \
    const __auto_type GET_MACRO_VARIABLE_NAME(oVectorLocal) = oVector; \
    GET_MACRO_VARIABLE_NAME(oVectorLocal).x == 0 && GET_MACRO_VARIABLE_NAME(oVectorLocal).y == 0; \
})

#define IS_2D_VECTOR_EQUAL(oVector1, oVector2) ({ \
    const __auto_type GET_MACRO_VARIABLE_NAME(oVector1Local) = oVector1; \
    const __auto_type GET_MACRO_VARIABLE_NAME(oVector2Local) = oVector2; \
    GET_MACRO_VARIABLE_NAME(oVector1Local).x == GET_MACRO_VARIABLE_NAME(oVector2Local).x && \
    GET_MACRO_VARIABLE_NAME(oVector1Local).y == GET_MACRO_VARIABLE_NAME(oVector2Local).y; \
})

#define LERP_INT(SourceValue, TargetValue, Balance, MaxBalance) ({ \
    const __auto_type GET_MACRO_VARIABLE_NAME(BalanceLocal) = Balance; \
    const __auto_type GET_MACRO_VARIABLE_NAME(MaxBalanceLocal) = MaxBalance; \
    ((SourceValue) * (GET_MACRO_VARIABLE_NAME(MaxBalanceLocal) - GET_MACRO_VARIABLE_NAME(BalanceLocal)) + (TargetValue) * GET_MACRO_VARIABLE_NAME(BalanceLocal) ) / GET_MACRO_VARIABLE_NAME(MaxBalanceLocal); \
})

#define LERP_F16(SourceValue, TargetValue, Balance) ({ \
    const __auto_type GET_MACRO_VARIABLE_NAME(BalanceLocal) = Balance; \
    F16_mul(SourceValue, FIX16(1) - GET_MACRO_VARIABLE_NAME(BalanceLocal)) + F16_mul(TargetValue, GET_MACRO_VARIABLE_NAME(BalanceLocal)); \
})

#define COPY_SIGN(SourceValue, TargetValue) ((((SourceValue) >= 0) == ((TargetValue) >= 0)) ? (TargetValue) : -(TargetValue))

f16 GetSinFromCos(f16 _fCos);
f16 GetAngleFromCos(f16 _fCos);
//Converts an angle in degrees to radians mapped to the [0..1024] range (used by trigonometric functions)
u16 GetIntegerRadiansFromDegrees(f16 _fAngle);
u16 GetAngleInIntegerRadians(s16 _sX, s16 _sY);
s32 DotProduct(const Vector3_s8* _pVector1, const Vector3_s8* _pVector2);

#endif //#ifndef _MATH_UTILS_H_