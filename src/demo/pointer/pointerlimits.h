#ifndef _POINTER_LIMITS_H_
#define _POINTER_LIMITS_H_

#include <genesis.h>

typedef V2u16 PointerLimitPositionCallback(void* _pData, const V2s16* _pPosition);

typedef struct
{
    void* m_pData;
    PointerLimitPositionCallback* OnLimitPosition;
} PointerLimits;

V2u16 GetPointerLimitedPosition(PointerLimits* _pPointerLimits, const V2s16* _pPosition);

#endif //#ifndef _POINTER_LIMITS_H_