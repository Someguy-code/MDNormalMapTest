#ifndef _POINTER_LIMITS_CIRCLE_H_
#define _POINTER_LIMITS_CIRCLE_H_

#include <demo/pointer/pointerlimits.h>

typedef struct
{
    u16 m_uRadius;
    V2u16 m_uCenter;
} PointerLimitsCircleData;

V2u16 PointerLimitPositionCircle(void* _pData, const V2s16* _pPosition);

#endif //#ifndef _POINTER_LIMITS_CIRCLE_H_