#include "statemachine.h"

void SetStateMachineState(StateMachine* _pStateMachine, const StateMachineState* _pState)
{
    const StateMachineState* pCurrentState = _pStateMachine->m_pCurrentState;
    if(pCurrentState != NULL)
    {
        OnExitState* pOnExit = pCurrentState->OnExit;
        if(pOnExit != NULL)
            pOnExit(_pStateMachine, pCurrentState, _pState);
    }
    
    _pStateMachine->m_pCurrentState = _pState;

    OnEnterState* pOnEnter = _pState->OnEnter;
    if(pOnEnter != NULL)
        pOnEnter(_pStateMachine, _pState, pCurrentState);

}

void UpdateStateMachine(StateMachine* _pStateMachine)
{
    const StateMachineState* pCurrentState = _pStateMachine->m_pCurrentState;
    if(pCurrentState == NULL)
        return;

    OnUpdateState* pOnUpdateState = pCurrentState->OnUpdate;
    if(pOnUpdateState == NULL)
        return;

    pOnUpdateState(_pStateMachine, pCurrentState);
}