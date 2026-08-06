#include "demo.h"

#include <demo/statemachine/stateloadingmain.h>
#include <demo/tutorial/tutorial.h>

#include <engine/engine.h>
#include <engine/input/mouseutils.h>
#include <engine/statemachine/statemachine.h>
#include <engine/statemachine/statemachinestate.h>

void InitializeDemo(Demo* _pDemo, struct Engine* _pEngine)
{
    *_pDemo = (Demo){
        .m_pEngine = _pEngine
    };

    EnableMouseSupport(PORT_2);

    DemoStateMachine* pDemoStateMachine = &_pDemo->m_oStateMachine;
    InitializeDemoStateMachine(pDemoStateMachine, &(DemoStateMachineParameters){
        .m_pCenter = &(V2u16){screenWidth / 2, screenHeight / 2},
        .m_pDemo = _pDemo,
        .m_pEngine = _pEngine,
        .m_pTutorial = &_pDemo->m_oTutorial,
        .m_pMaterialModifierController = &_pDemo->m_oMaterialModifierController,
    });
    
    SetStateMachineState(&pDemoStateMachine->m_oStateMachine, &pDemoStateMachine->m_oLoadingTitleState.m_oState);
}

void UpdateDemo(Demo* _pDemoData)
{
    UpdateStateMachine(&_pDemoData->m_oStateMachine.m_oStateMachine);
}