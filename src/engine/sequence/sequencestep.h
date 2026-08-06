#ifndef _SEQUENCE_STEP_H_
#define _SEQUENCE_STEP_H_

#include <genesis.h>

struct Sequence;
struct SequenceStep;

typedef void OnSequenceStepExecute(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);

typedef struct SequenceStep
{
    void* m_pData;
    u16 m_uDelay;
    OnSequenceStepExecute* OnExecute;
} SequenceStep;

#endif //#ifndef _SEQUENCE_STEP_H_