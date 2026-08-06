#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <genesis.h>

#include "statemachinestate.h"

typedef struct StateMachine
{
    void* m_pData;
    const StateMachineState* m_pCurrentState;
} StateMachine;

void SetStateMachineState(StateMachine* _pStateMachine, const StateMachineState* _pState);
void UpdateStateMachine(StateMachine* _pStateMachine);

#endif //#ifndef _STATE_MACHINE_H_