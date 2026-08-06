#include "pointerlimits.h"

FORCE_INLINE V2u16 GetPointerLimitedPosition(PointerLimits* _pPointerLimits, const V2s16* _pPosition)
{
    PointerLimitPositionCallback* pOnLimitPosition = _pPointerLimits->OnLimitPosition;
    return pOnLimitPosition != NULL ? pOnLimitPosition(_pPointerLimits->m_pData, _pPosition) : (V2u16){_pPosition->x, _pPosition->y};
}