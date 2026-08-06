#include "demostatemachine.h"

#include <demo/demo.h>

void InitializeDemoStateMachine(DemoStateMachine* _pDemoStateMachine, const DemoStateMachineParameters* _pParameters)
{
    *_pDemoStateMachine = (DemoStateMachine){};

    InitializeStateLoadingTitle(&_pDemoStateMachine->m_oLoadingTitleState, &(StateLoadingTitleParameters){
        .m_pCenter =  _pParameters->m_pCenter,
        .m_pNextState = &_pDemoStateMachine->m_oTitleState.m_oState,
        .m_pEngine = _pParameters->m_pEngine,
    });
    InitializeStateTitle(&_pDemoStateMachine->m_oTitleState, &(StateTitleParameters){
        .m_pNextState = &_pDemoStateMachine->m_oLoadingMainState.m_oState,
        .m_pEngine = _pParameters->m_pEngine,
    });
    InitializeStateLoadingMain(&_pDemoStateMachine->m_oLoadingMainState, &(StateLoadingMainParameters){
        .m_pCenter =  _pParameters->m_pCenter,
        .m_pOutMaterial = &_pParameters->m_pDemo->m_oMaterial,
        .m_pOutPointer = &_pParameters->m_pDemo->m_oPointer,
        .m_pOutTutorial = &_pParameters->m_pDemo->m_oTutorial,
        .m_pOutMaterialModifierController = &_pParameters->m_pDemo->m_oMaterialModifierController,
        .m_pNextState = &_pDemoStateMachine->m_oOrbitState.m_oState,
        .m_pEngine = _pParameters->m_pEngine,
    });
    InitializeStateOrbit(&_pDemoStateMachine->m_oOrbitState, &(StateOrbitParameters){
        .m_pCenter =  _pParameters->m_pCenter,
        .m_uOrbitRadius = 90,
        .m_pMaterial = &_pParameters->m_pDemo->m_oMaterial,
        .m_pPointer = &_pParameters->m_pDemo->m_oPointer,
        .m_pTutorial = &_pParameters->m_pDemo->m_oTutorial,
        .m_pMaterialModifierController = &_pParameters->m_pDemo->m_oMaterialModifierController,
        .m_pManualControlStateState = &_pDemoStateMachine->m_oManualControlState.m_oState,
        .m_pEngine = _pParameters->m_pEngine,
    });
    IniatilizeStateManualControl(&_pDemoStateMachine->m_oManualControlState, &(StateManualControlParameters){
        .m_pCenter =  _pParameters->m_pCenter,
        .m_uOrbitRadius = 90,
        .m_pMaterial = &_pParameters->m_pDemo->m_oMaterial,
        .m_pPointer = &_pParameters->m_pDemo->m_oPointer,
        .m_pTutorial = &_pParameters->m_pDemo->m_oTutorial,
        .m_pMaterialModifierController = &_pParameters->m_pDemo->m_oMaterialModifierController,
        .m_pOrbitState = &_pDemoStateMachine->m_oOrbitState.m_oState,
        .m_pEngine = _pParameters->m_pEngine,
    });
}