#include "stateorbit.h"

#include <demo/materialmodifiercontroller/materialmodifiercontroller.h>
#include <demo/pointer/pointer.h>
#include <demo/utils/lightutils.h>
#include <demo/tutorial/tutorial.h>

#include <engine/engine.h>
#include <engine/material/material.h>
#include <engine/statemachine/statemachine.h>
#include <engine/types/int16interpolator.h>
#include <engine/utils/mathutils.h>

//Make a full orbit in 3 seconds
static const u16 MAX_ORBIT_SPEED = 1024 / (3*60);
static const u16 FRAMES_TO_REACH_MAX_SPEED = 60;

static void OnEnterOrbit(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState);
static void OnUpdateOrbit(StateMachine* _pStateMachine, const StateMachineState* _pState);

static void JoyHandler(void* _pData, u16 _uJoyID, u16 _uChanged, u16 _uState);
static void ComputeInitialAngleAndTilt(StateOrbitData* _pOrbitData);
static void UpdateOrbitTiltChangeTimer(StateOrbitData* _pOrbitData);
static u16 GetOrbitTilt(StateOrbitData* _pOrbitData);
static bool CanSwitchToManualControl(const StateOrbitData* _pOrbitData);

void InitializeStateOrbit(StateOrbit* _pStateOrbit, const StateOrbitParameters* _pParameters)
{
    *_pStateOrbit = (StateOrbit) {
        .m_oData = (StateOrbitData){
            .m_oCenter = *_pParameters->m_pCenter,
            .m_fOrbitRadius = FIX16(_pParameters->m_uOrbitRadius),
            .m_uCurrentAngle = 0,
            .m_oOrbitSpeedInterpolator = {},
            .m_oTiltInterpolator = {},
            .m_uTiltChangeFramesLeft = 0,
            .m_pMaterial = _pParameters->m_pMaterial,
            .m_pPointer = _pParameters->m_pPointer,
            .m_pTutorial = _pParameters->m_pTutorial,
            .m_pMaterialModifierController = _pParameters->m_pMaterialModifierController,
            .m_pManualControlStateState = _pParameters->m_pManualControlStateState,
            .m_pEngine = _pParameters->m_pEngine,
        },
        .m_oState = (StateMachineState) {
            .m_pData = &_pStateOrbit->m_oData,
            .OnEnter = OnEnterOrbit,
            .OnUpdate = OnUpdateOrbit,
        }
    };
}

static void OnEnterOrbit(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState)
{
    StateOrbitData* pOrbitData = _pState->m_pData;
    SetPointerLimits(pOrbitData->m_pPointer, &NullPointerLimits);
    SetInputManagerCallback(&pOrbitData->m_pEngine->m_oInputManager, pOrbitData, JoyHandler);
    InitializeU16Interpolator(&pOrbitData->m_oOrbitSpeedInterpolator, 0, MAX_ORBIT_SPEED, FRAMES_TO_REACH_MAX_SPEED);
    if(_pLastState == pOrbitData->m_pManualControlStateState)
        ComputeInitialAngleAndTilt(pOrbitData);
}

static void OnUpdateOrbit(StateMachine* _pStateMachine, const StateMachineState* _pState)
{
    StateOrbitData* pOrbitData = _pState->m_pData;

    if(CanSwitchToManualControl(pOrbitData))
    {
        SetStateMachineState(_pStateMachine, pOrbitData->m_pManualControlStateState);
        return;
    }

    u16* uCurrentAngle = &pOrbitData->m_uCurrentAngle;
    *uCurrentAngle += UpdateU16Interpolator(&pOrbitData->m_oOrbitSpeedInterpolator);
    const u16 uTilt = GetOrbitTilt(pOrbitData);
    const Vect3D_f16 oLightDirection = {
        .x = -F16_mul(cosFix16(*uCurrentAngle), cosFix16(uTilt)),
        .y = -F16_mul(cosFix16(*uCurrentAngle), sinFix16(uTilt)),
        .z = -sinFix16(*uCurrentAngle)
    };

    pOrbitData->m_fLastDirectionZ = oLightDirection.z;

    const V2u16* pCenter = &pOrbitData->m_oCenter;
    const f16 fOrbitRadius = pOrbitData->m_fOrbitRadius;
    const V2u16 oOrbitPosition = {
        .x = pCenter->x - F16_toRoundedInt(F16_mul(oLightDirection.x, fOrbitRadius)),
        .y = pCenter->y + F16_toRoundedInt(F16_mul(oLightDirection.y, fOrbitRadius)),
    };

    SetPointerPosition(pOrbitData->m_pPointer, &oOrbitPosition, oLightDirection.z < 0);

    (void) UpdateMaterialModifierController(pOrbitData->m_pMaterialModifierController);

    const Vector3_s8 oLightDirectionInteger = GetLightDirectionS8(&oLightDirection);
    const Material* pMaterial = &pOrbitData->m_pMaterialModifierController->m_oMutabledMaterialWrapper.m_oMaterial;
    UpdateMaterial(pMaterial, &oLightDirectionInteger, &pOrbitData->m_pEngine->m_oPaletteManager);

    UpdateTutorial(pOrbitData->m_pTutorial);
}

static void JoyHandler(void* _pData, u16 _uJoyID, u16 _uChanged, u16 _uState)
{
    StateOrbitData* pStateOrbitData = (StateOrbitData*)_pData;
    JoyHandlerMaterialModifierController(pStateOrbitData->m_pMaterialModifierController, _uJoyID, _uChanged, _uState);
    PointerJoyEventHandler(pStateOrbitData->m_pPointer, _uJoyID, _uChanged, _uState);
}

static void ComputeInitialAngleAndTilt(StateOrbitData* _pOrbitData)
{
    const Pointer* pPointer = _pOrbitData->m_pPointer;
    const V2u16* pCenter = &_pOrbitData->m_oCenter;
    const V2u16* pPointerPosition = &pPointer->m_oPosition;
    u16 uCurrentAngle = 1024 / 4;
    u16 uTilt = 0;
    const V2s16 oPositionToCenterDelta = {pPointerPosition->x - pCenter->x, pPointerPosition->y - pCenter->y};
    if(!IS_2D_VECTOR_NULL(oPositionToCenterDelta))
    {
        uTilt = GetAngleInIntegerRadians(oPositionToCenterDelta.x, -oPositionToCenterDelta.y);
        u16 uDistance = getApproximatedDistance(oPositionToCenterDelta.x, oPositionToCenterDelta.y);
        const u16 uOrbitRadius = F16_toRoundedInt(_pOrbitData->m_fOrbitRadius);
        uDistance = clamp(uDistance, 0, uOrbitRadius);
        const f16 fCos = F16_div(FIX16(uDistance), _pOrbitData->m_fOrbitRadius);
        uCurrentAngle = GetIntegerRadiansFromDegrees(GetAngleFromCos(fCos));
    }

    _pOrbitData->m_uCurrentAngle = uCurrentAngle;
    _pOrbitData->m_oTiltInterpolator.m_uTargetValue = uTilt;
    _pOrbitData->m_uTiltChangeFramesLeft = 0;
}

static void UpdateOrbitTiltChangeTimer(StateOrbitData* _pOrbitData)
{
    u16 uTiltChangeFramesLeft = _pOrbitData->m_uTiltChangeFramesLeft;
    if(uTiltChangeFramesLeft > 0)
        --uTiltChangeFramesLeft;
    else
    {
        const u16 uTiltChangeFrames = 3 * 60;
        uTiltChangeFramesLeft = uTiltChangeFrames;
        const u16 uMaxTiltVariation = 1024 / 8;
        const s16 sTiltInterpolationOffset = random() % (2 * uMaxTiltVariation) - uMaxTiltVariation;
        const u16 uTiltInterpolationFramesCount = uTiltChangeFrames;
        u16Interpolator* pTiltInterpolator = &_pOrbitData->m_oTiltInterpolator;
        const u16 uCurrentTilt = pTiltInterpolator->m_uTargetValue;
        InitializeU16Interpolator(pTiltInterpolator, uCurrentTilt, uCurrentTilt + sTiltInterpolationOffset, uTiltInterpolationFramesCount);
    }
    
    _pOrbitData->m_uTiltChangeFramesLeft = uTiltChangeFramesLeft;
}

static u16 GetOrbitTilt(StateOrbitData* _pOrbitData)
{
    UpdateOrbitTiltChangeTimer(_pOrbitData);
    return UpdateU16Interpolator(&_pOrbitData->m_oTiltInterpolator);
}

static bool CanSwitchToManualControl(const StateOrbitData* _pOrbitData)
{
    return !IS_2D_VECTOR_NULL(GetPointerSpeed(_pOrbitData->m_pPointer, false));
}