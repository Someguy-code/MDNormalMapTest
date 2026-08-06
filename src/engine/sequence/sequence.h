#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include <genesis.h>

struct SequenceStep;

//Executes a squence of callbacks, each with a delay in frames relative to the previous
typedef struct Sequence
{
    const struct SequenceStep* m_pSequenceElements;
    u16 m_uSequenceElementsCount;
    u16 m_uNextSequenceElementIndex;
    u16 m_uFramesLeftForNextElement;
} Sequence;


void ResetSequence(Sequence* _pSequence);
void JumpToSequenceStep(Sequence* _pSequence, u16 _uTargetStepIndex, u16 _uDelay);
bool IsSequenceStepDone(const Sequence* _pSequence, u16 _uStepIndex);
bool UpdateSequence(Sequence* _pSequence, void* _pContextData);

#endif //#ifndef _SEQUENCE_H_