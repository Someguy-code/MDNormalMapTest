#include "pointerlimitscircle.h"

V2u16 PointerLimitPositionCircle(void* _pData, const V2s16* _pPosition)
{
    PointerLimitsCircleData* pData = _pData;
    const V2u16* pCenter = &pData->m_uCenter;
    const V2s16 oDeltaToCenter = {_pPosition->x - pCenter->x, _pPosition->y - pCenter->y};
    //NOTE: Using getApproximatedDistance will result in an octagon, rather than a circle
    const u32 uDistanceToCenter = getApproximatedDistance(oDeltaToCenter.x, oDeltaToCenter.y);
    const u16 uRadius = pData->m_uRadius;
    return uDistanceToCenter <= uRadius ?
        (V2u16){_pPosition->x, _pPosition->y} :
        (V2u16){
            pCenter->x + ((s16)uRadius * oDeltaToCenter.x) / (s16)uDistanceToCenter,
            pCenter->y + ((s16)uRadius * oDeltaToCenter.y) / (s16)uDistanceToCenter
        };
}