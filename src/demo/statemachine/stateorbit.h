#ifndef _DEMO_STATE_ORBIT_H_
#define _DEMO_STATE_ORBIT_H_

#include <engine/statemachine/statemachinestate.h>
#include <engine/types/int16interpolator.h>
#include <engine/types/vector3.h>

struct Material;
struct Pointer;
struct Tutorial;
struct MaterialModifierController;

typedef struct
{
    const V2u16* m_pCenter;
    u16 m_uOrbitRadius;
    struct Material* m_pMaterial;
    struct Pointer* m_pPointer;
    struct Tutorial* m_pTutorial;
    struct MaterialModifierController* m_pMaterialModifierController;
    StateMachineState* m_pManualControlStateState;

    struct Engine* m_pEngine;
} StateOrbitParameters;

typedef struct
{
    V2u16 m_oCenter;
    f16 m_fOrbitRadius;
    u16 m_uCurrentAngle;
    u16Interpolator m_oOrbitSpeedInterpolator;
    u16Interpolator m_oTiltInterpolator;
    u16 m_uTiltChangeFramesLeft;
    //Necessary to pass the current direction Z to the manual control state
    f16 m_fLastDirectionZ;

    struct Material* m_pMaterial;
    struct Pointer* m_pPointer;
    struct Tutorial* m_pTutorial;
    struct MaterialModifierController* m_pMaterialModifierController;
    StateMachineState* m_pManualControlStateState;

    struct Engine* m_pEngine;
} StateOrbitData;

typedef struct StateOrbit
{
    StateOrbitData m_oData;
    StateMachineState m_oState;
} StateOrbit;

void InitializeStateOrbit(StateOrbit* _pStateOrbit, const StateOrbitParameters* _pStateOrbitParameters);

#endif //#ifndef _DEMO_STATE_ORBIT_H_