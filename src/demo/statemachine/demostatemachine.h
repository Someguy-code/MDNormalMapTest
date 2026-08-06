#ifndef _DEMO_STATE_MACHINE_H_
#define _DEMO_STATE_MACHINE_H_

#include <demo/statemachine/stateloadingtitle.h>
#include <demo/statemachine/statetitle.h>
#include <demo/statemachine/stateloadingmain.h>
#include <demo/statemachine/stateorbit.h>
#include <demo/statemachine/statemanualcontrol.h>

#include <engine/statemachine/statemachine.h>
#include <engine/statemachine/statemachinestate.h>

struct Demo;
struct Engine;
struct Tutorial;
struct MaterialModifierController;

typedef struct DemoStateMachineParameters
{
    const V2u16* m_pCenter;
    struct Demo* m_pDemo;
    struct Engine* m_pEngine;
    struct Tutorial* m_pTutorial;
    struct MaterialModifierController* m_pMaterialModifierController;
}DemoStateMachineParameters;

typedef struct DemoStateMachine
{
    StateMachine m_oStateMachine;
    StateLoadingTitle m_oLoadingTitleState;
    StateTitle m_oTitleState;
    StateLoadingMain m_oLoadingMainState;
    StateOrbit m_oOrbitState;
    StateManualControl m_oManualControlState;
} DemoStateMachine;

void InitializeDemoStateMachine(DemoStateMachine* _pDemoStateMachine, const DemoStateMachineParameters* _pParameters);

#endif //#ifndef _DEMO_STATE_MACHINE_H_