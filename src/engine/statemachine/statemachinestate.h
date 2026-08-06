#ifndef _STATE_MACHINE_STATE_H_
#define _STATE_MACHINE_STATE_H_

#include <genesis.h>

struct StateMachine;
struct StateMachineState;

typedef void OnEnterState(struct StateMachine* _pStateMachine, const struct StateMachineState* _pState, const struct StateMachineState* _pLastState);
typedef void OnExitState(struct StateMachine* _pStateMachine, const struct StateMachineState* _pState, const struct StateMachineState* _pNextState);
typedef void OnUpdateState(struct StateMachine* _pStateMachine, const struct StateMachineState* _pState);

typedef struct StateMachineState
{
    void* m_pData;
    OnEnterState* OnEnter;
    OnExitState* OnExit;
    OnUpdateState* OnUpdate;
} StateMachineState;

#endif //#ifndef _STATE_MACHINE_STATE_H_