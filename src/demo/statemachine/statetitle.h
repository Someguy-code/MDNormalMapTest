#ifndef _DEMO_STATE_TITLE_H_
#define _DEMO_STATE_TITLE_H_

#include <engine/statemachine/statemachinestate.h>
#include <engine/sequence/sequencestep.h>
#include <engine/sequence/sequence.h>

struct Engine;
struct StateMachine;

typedef struct
{
    StateMachineState* m_pNextState;
    struct Engine* m_pEngine;
} StateTitleParameters;

typedef struct
{
    StateMachineState* m_pNextState;

    SequenceStep m_oSequenceElements[7];
    Sequence m_oSequence;
    bool m_bIsEarlyExit;

    struct Engine* m_pEngine;
} StateTitleData;

typedef struct StateTitle
{
    StateTitleData m_oData;
    StateMachineState m_oState;
} StateTitle;

void InitializeStateTitle(StateTitle* _pStateTitle, const StateTitleParameters* _pParameters);

#endif //#ifndef _DEMO_STATE_TITLE_H_