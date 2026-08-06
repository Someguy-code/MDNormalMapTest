#ifndef _DEMO_STATE_LOADING_H_
#define _DEMO_STATE_LOADING_H_

#include <engine/statemachine/statemachinestate.h>

struct Material;
struct Pointer;
struct Engine;
struct Tutorial;
struct MaterialModifierController;

typedef struct
{
    const V2u16* m_pCenter;
    StateMachineState* m_pNextState;
    //Output parameters (what is going to be loaded)
    struct Material* m_pOutMaterial;
    struct Pointer* m_pOutPointer;
    struct Tutorial* m_pOutTutorial;
    struct MaterialModifierController* m_pOutMaterialModifierController;

    struct Engine* m_pEngine;
} StateLoadingMainParameters;

typedef struct
{
    V2u16 m_oCenter;
    
    StateMachineState* m_pNextState;
    struct Material* m_pOutMaterial;
    struct Pointer* m_pOutPointer;
    struct Tutorial* m_pOutTutorial;
    struct MaterialModifierController* m_pOutMaterialModifierController;

    struct Engine* m_pEngine;
} StateLoadingMainData;

typedef struct StateLoadingMain
{
    StateLoadingMainData m_oData;
    StateMachineState m_oState;
} StateLoadingMain;

void InitializeStateLoadingMain(StateLoadingMain* _pStateLoading, const StateLoadingMainParameters* _pParameters);

#endif //#ifndef _DEMO_STATE_LOADING_H_