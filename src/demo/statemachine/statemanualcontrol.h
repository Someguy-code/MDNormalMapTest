#ifndef _DEMO_STATE_MANUAL_CONTROL_H_
#define _DEMO_STATE_MANUAL_CONTROL_H_

#include <demo/pointer/pointerlimitscircle.h>

#include <engine/statemachine/statemachinestate.h>
#include <engine/types/f16interpolator.h>
#include <engine/types/vector3.h>

struct Material;
struct Pointer;
struct Engine;
struct MaterialModifierController;

typedef struct
{
    const V2u16* m_pCenter;
    u16 m_uOrbitRadius;
    StateMachineState* m_pOrbitState;
    struct Material* m_pMaterial;
    struct Pointer* m_pPointer;
    struct Tutorial* m_pTutorial;
    struct MaterialModifierController* m_pMaterialModifierController;

    struct Engine* m_pEngine;
} StateManualControlParameters;

typedef struct
{
    V2u16 m_oCenter;
    u16 m_uOrbitRadius;
    StateMachineState* m_pOrbitState;
    struct Material* m_pMaterial;
    struct Pointer* m_pPointer;
    struct Tutorial* m_pTutorial;
    struct MaterialModifierController* m_pMaterialModifierController;

    bool m_bIsLastDirectionValid;
    Vector3_s8 m_oLastLightDirection; 
    PointerLimitsCircleData m_oPointerLimits;

    u16 m_uFramesToGoToOrbitLeft;

    //When regaining control, if the light was behind the model, interpolate the direction until it faces front
    f16Interpolator m_oFromOrbitDirectionZInterpolator;

    struct Engine* m_pEngine;
} StateManualControlData;

typedef struct StateManualControl
{
    StateManualControlData m_oData;
    StateMachineState m_oState;
} StateManualControl;

void IniatilizeStateManualControl(StateManualControl* _pStateManualControl, const StateManualControlParameters* _pParameters);

#endif //#ifndef _DEMO_STATE_MANUAL_CONTROL_H_