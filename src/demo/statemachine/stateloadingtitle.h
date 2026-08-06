#ifndef _DEMO_STATE_LOADING_TITLE_CONTROL_H_
#define _DEMO_STATE_LOADING_TITLE_CONTROL_H_

#include <engine/statemachine/statemachinestate.h>

struct Engine;

typedef struct
{
    const V2u16* m_pCenter;
    StateMachineState* m_pNextState;

    struct Engine* m_pEngine;
} StateLoadingTitleParameters;

typedef struct
{
    V2u16 m_oCenter;
    
    StateMachineState* m_pNextState;

    struct Engine* m_pEngine;
} StateLoadingTitleData;

typedef struct StateLoadingTitle
{
    StateLoadingTitleData m_oData;
    StateMachineState m_oState;
} StateLoadingTitle;

void InitializeStateLoadingTitle(StateLoadingTitle* _pStateLoadingTitle, const StateLoadingTitleParameters* _pParameters);

#endif //#ifndef _DEMO_STATE_LOADING_TITLE_CONTROL_H_