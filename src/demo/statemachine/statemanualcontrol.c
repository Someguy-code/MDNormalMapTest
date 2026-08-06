#include "statemanualcontrol.h"

#include <demo/statemachine/stateorbit.h>
#include <demo/materialmodifiercontroller/materialmodifiercontroller.h>
#include <demo/pointer/pointer.h>
#include <demo/tutorial/tutorial.h>
#include <demo/utils/lightutils.h>

#include <engine/engine.h>
#include <engine/material/material.h>
#include <engine/input/inputmanager.h>
#include <engine/input/mouseutils.h>
#include <engine/statemachine/statemachine.h>
#include <engine/types/vector3.h>
#include <engine/utils/mathutils.h>

//Go back to orbit after 3 seconds of inactivity
static const u16 FRAMES_TO_GO_TO_ORBIT = 60 * 2;

static const u16 FRAMES_TO_INTERPOLATE_DIRECTION_Z = 20;

static void OnEnterManualControl(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState);
static void OnUpdateManualControl(StateMachine* _pStateMachine, const StateMachineState* _pState);

static Vect3D_f16 GetCurrentLightDirection(StateManualControlData* _pStateManualControlData);

static void JoyHandler(void* _pData, u16 _uJoyID, u16 _uChanged, u16 _uState);

static void InitializeGoToOrbit(StateManualControlData* _pStateManualControlData);
static bool UpdateGoToOrbit(StateManualControlData* _pStateManualControlData);

static void InitializeFromOrbitDirectionZInterpolation(StateManualControlData* _pStateManualControlData);
static Vect3D_f16 UpdateFromOrbitDirectionZInterpolation(StateManualControlData* _pStateManualControlData, const Vect3D_f16* _pCurrentLightDirection);
static bool MustUpdateMaterial(const StateManualControlData* _pStateManualControlData, const Vector3_s8* _pCurrentLightDirection, const Vector3_s8* _pLastLightDirection, bool _bMaterialModified);

void IniatilizeStateManualControl(StateManualControl* _pStateManualControl, const StateManualControlParameters* _pParameters)
{
    *_pStateManualControl = (StateManualControl){
        .m_oData = {
            .m_oCenter = *_pParameters->m_pCenter,
            .m_uOrbitRadius = _pParameters->m_uOrbitRadius,
            .m_pOrbitState = _pParameters->m_pOrbitState,
            .m_pMaterial = _pParameters->m_pMaterial,
            .m_pPointer = _pParameters->m_pPointer,
            .m_pTutorial = _pParameters->m_pTutorial,
            .m_pMaterialModifierController = _pParameters->m_pMaterialModifierController,
            .m_oPointerLimits = {
                .m_uCenter = *_pParameters->m_pCenter,
                .m_uRadius = _pParameters->m_uOrbitRadius,
            },
            .m_pEngine = _pParameters->m_pEngine,
        },
        .m_oState = (StateMachineState){
            .m_pData = &_pStateManualControl->m_oData,
            .OnEnter = OnEnterManualControl,
            .OnUpdate = OnUpdateManualControl,
        }
    };
}

static void OnEnterManualControl(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState)
{
    StateManualControlData* pStateManualControlData = _pState->m_pData;
    SetInputManagerCallback(&pStateManualControlData->m_pEngine->m_oInputManager, pStateManualControlData, JoyHandler);
    SetPointerLimits(pStateManualControlData->m_pPointer, &(PointerLimits){
        .m_pData = &pStateManualControlData->m_oPointerLimits,
        .OnLimitPosition = PointerLimitPositionCircle});
    InitializeGoToOrbit(pStateManualControlData);
    InitializeFromOrbitDirectionZInterpolation(pStateManualControlData);
    pStateManualControlData->m_bIsLastDirectionValid = false;
}

static void OnUpdateManualControl(StateMachine* _pStateMachine, const StateMachineState* _pState)
{
    StateManualControlData* pStateManualControlData = _pState->m_pData;

    if(UpdateGoToOrbit(pStateManualControlData))
    {
        SetStateMachineState(_pStateMachine, pStateManualControlData->m_pOrbitState);
        return;
    }

    Pointer* pPointer = pStateManualControlData->m_pPointer;
    UpdatePointer(pPointer);

    const bool bMaterialModified = UpdateMaterialModifierController(pStateManualControlData->m_pMaterialModifierController);

    const Vect3D_f16 oLightDirection = GetCurrentLightDirection(pStateManualControlData);
    const Vector3_s8 oLightDirectionInteger = GetLightDirectionS8(&oLightDirection);
    Vector3_s8* pLastLightDirection = &pStateManualControlData->m_oLastLightDirection;
    if(MustUpdateMaterial(pStateManualControlData, &oLightDirectionInteger, pLastLightDirection, bMaterialModified))
    {
        const Material* pMaterial = &pStateManualControlData->m_pMaterialModifierController->m_oMutabledMaterialWrapper.m_oMaterial;
        UpdateMaterial(pMaterial, &oLightDirectionInteger, &pStateManualControlData->m_pEngine->m_oPaletteManager);
    }
    
    UpdateTutorial(pStateManualControlData->m_pTutorial);

    pStateManualControlData->m_bIsLastDirectionValid = true;
    *pLastLightDirection = oLightDirectionInteger;
}

static Vect3D_f16 GetCurrentLightDirection(StateManualControlData* _pStateManualControlData)
{
    Pointer* pPointer = _pStateManualControlData->m_pPointer;
    const V2u16* pPointerPosition = &pPointer->m_oPosition;
    const V2u16* pCenter = &_pStateManualControlData->m_oCenter;
    const Vect3D_f16 oCurrentLightDirection = GetLightDirection(pPointerPosition->x - pCenter->x, pPointerPosition->y - pCenter->y, pCenter->y);
    return UpdateFromOrbitDirectionZInterpolation(_pStateManualControlData, &oCurrentLightDirection);
}

static void JoyHandler(void* _pData, u16 _uJoyID, u16 _uChanged, u16 _uState)
{
    StateManualControlData* pStateManualControlData = (StateManualControlData*)_pData;
    JoyHandlerMaterialModifierController(pStateManualControlData->m_pMaterialModifierController, _uJoyID, _uChanged, _uState);
    PointerJoyEventHandler(pStateManualControlData->m_pPointer, _uJoyID, _uChanged, _uState);
}

static void InitializeGoToOrbit(StateManualControlData* _pStateManualControlData)
{
    _pStateManualControlData->m_uFramesToGoToOrbitLeft = FRAMES_TO_GO_TO_ORBIT;
    ResetMouseSpeed(JOY_2);
}

static bool UpdateGoToOrbit(StateManualControlData* _pStateManualControlData)
{
    const Pointer* pPointer = _pStateManualControlData->m_pPointer;
    u16* pFramesToGoToOrbitLeft = &_pStateManualControlData->m_uFramesToGoToOrbitLeft;

    if(!IS_2D_VECTOR_NULL(GetPointerSpeed(pPointer, false)))
        *pFramesToGoToOrbitLeft = FRAMES_TO_GO_TO_ORBIT;
    else if(*pFramesToGoToOrbitLeft > 0)
        --*pFramesToGoToOrbitLeft;

    return *pFramesToGoToOrbitLeft == 0;
}

static void InitializeFromOrbitDirectionZInterpolation(StateManualControlData* _pStateManualControlData)
{
    const f16 fLastDirectionZ = ((StateOrbitData*)_pStateManualControlData->m_pOrbitState->m_pData)->m_fLastDirectionZ;
    //If it's light direction comes from behind, start the interpolation. Do not worry about the target, it will be computed on update.
    if(fLastDirectionZ > 0)
        InitializeF16Interpolator(&_pStateManualControlData->m_oFromOrbitDirectionZInterpolator, fLastDirectionZ, 0, FRAMES_TO_INTERPOLATE_DIRECTION_Z);
}

static Vect3D_f16 UpdateFromOrbitDirectionZInterpolation(StateManualControlData* _pStateManualControlData, const Vect3D_f16* _pCurrentLightDirection)
{
    Vect3D_f16 oInterpolatedLightDirection = *_pCurrentLightDirection;
    f16Interpolator* pFromOrbitDirectionZInterpolator = &_pStateManualControlData->m_oFromOrbitDirectionZInterpolator;
    if(!IsF16InterpolatorFinished(pFromOrbitDirectionZInterpolator))
    {
        pFromOrbitDirectionZInterpolator->m_fTargetValue = oInterpolatedLightDirection.z;
        const f16 fTargetZ = UpdateF16Interpolator(pFromOrbitDirectionZInterpolator);
        const f16 fHorizonatlMultiplier = GetSinFromCos(fTargetZ);
        oInterpolatedLightDirection = (Vect3D_f16){
            .x = F16_mul(oInterpolatedLightDirection.x, fHorizonatlMultiplier),
            .y = F16_mul(oInterpolatedLightDirection.y, fHorizonatlMultiplier),
            .z = fTargetZ,
        };
    }
    return oInterpolatedLightDirection;
}

static bool MustUpdateMaterial(const StateManualControlData* _pStateManualControlData, const Vector3_s8* _pCurrentLightDirection, const Vector3_s8* _pLastLightDirection, bool _bMaterialModified)
{
    return _bMaterialModified ||
        !_pStateManualControlData->m_bIsLastDirectionValid ||
        _pLastLightDirection->m_sX != _pCurrentLightDirection->m_sX ||
        _pLastLightDirection->m_sY != _pCurrentLightDirection->m_sY ||
        _pLastLightDirection->m_sZ != _pCurrentLightDirection->m_sZ;
}