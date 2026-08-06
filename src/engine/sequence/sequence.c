#include "sequence.h"

#include <engine/sequence/sequencestep.h>

void ResetSequence(Sequence* _pSequence)
{
    _pSequence->m_uNextSequenceElementIndex = 0;
    _pSequence->m_uFramesLeftForNextElement = _pSequence->m_uSequenceElementsCount > 0 ?
        _pSequence->m_pSequenceElements[0].m_uDelay : 0;
}

void JumpToSequenceStep(Sequence* _pSequence, u16 _uTargetStepIndex, u16 _uDelay)
{
    assert(_uTargetStepIndex < _pSequence->m_uSequenceElementsCount);
    _pSequence->m_uFramesLeftForNextElement = _uDelay;
    _pSequence->m_uNextSequenceElementIndex = _uTargetStepIndex;
}

bool IsSequenceStepDone(const Sequence* _pSequence, u16 _uStepIndex)
{
    return _pSequence->m_uNextSequenceElementIndex > _uStepIndex;
}

bool UpdateSequence(Sequence* _pSequence, void* _pContextData)
{
    u16* pFramesLeftForNextElement = &_pSequence->m_uFramesLeftForNextElement;
    u16* pNextSequenceElementIndex = &_pSequence->m_uNextSequenceElementIndex;
    const u16 uSequenceElementsCount = _pSequence->m_uSequenceElementsCount;
    while(*pFramesLeftForNextElement == 0 && *pNextSequenceElementIndex < uSequenceElementsCount)
    {
        const SequenceStep* pCurrentElement = &_pSequence->m_pSequenceElements[(*pNextSequenceElementIndex)++];
        pCurrentElement->OnExecute(_pSequence, pCurrentElement, _pContextData);
        if(*pNextSequenceElementIndex < uSequenceElementsCount)
            *pFramesLeftForNextElement = _pSequence->m_pSequenceElements[*pNextSequenceElementIndex].m_uDelay;
    }

    const bool bIsSequenceFinished = *pNextSequenceElementIndex == uSequenceElementsCount;
    if(!bIsSequenceFinished)
        --*pFramesLeftForNextElement;

    return bIsSequenceFinished;
}