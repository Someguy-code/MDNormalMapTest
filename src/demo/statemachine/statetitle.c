#include "statetitle.h"

#include <demo/pointer/pointer.h>

#include <engine/engine.h>
#include <engine/constants/colorconstants.h>
#include <engine/input/mouseutils.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/palettemanager/palettecomponentlimitfade.h>
#include <engine/palettemanager/palettelinearfade.h>
#include <engine/statemachine/statemachine.h>
#include <engine/utils/arrayutils.h>
#include <engine/utils/colorutils.h>

#include <resources.h>

enum TitleScreenConstants
{
    FADE_IN_FRAMES_TITLE = 21,
    FADE_IN_FRAMES_POWERED_BY = 21,
    FADE_IN_FRAMES_SGDK = 8,
    FADE_TO_WHITE_FRAMES_SGDK = 8,
    FADE_FROM_WHITE_FRAMES_SGDK = 16,
    INTER_FADE_FRAMES = 20,
    WAIT_FRAMES = 3 * 60,
    FADE_OUT_FRAMES = 21,
};

enum TitleSequenceSteps
{
    STEP_FADE_IN_TITLE,
    STEP_FADE_IN_POWERED_BY,
    STEP_FADE_IN_SGDK,
    STEP_FADE_TO_WHITE_SGDK,
    STEP_FADE_FROM_WHITE_SGDK,
    STEP_FADE_OUT,
    STEP_GO_TO_NEXT_STATE
};

static void OnEnterTitle(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState);
static void OnExitTitle(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState);
static void OnUpdateTitle(StateMachine* _pStateMachine, const StateMachineState* _pState);

static void StartFadeInTitle(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void StartFadeInPoweredBy(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void StartFadeInSGDK(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void StartFadeToWhiteSGDK(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void StartFadeFromWhiteSGDK(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void StartFadeOut(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void GoToNextState(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);

static void JoyHandler(void* _pData, u16 _uJoyID, u16 _uChanged, u16 _uState);

static bool CanEarlyExit(const StateTitleData* _pData, const Sequence* pSequence);

void InitializeStateTitle(StateTitle* _pStateTitle, const StateTitleParameters* _pParameters)
{
    *_pStateTitle = (StateTitle) {
        .m_oData = (StateTitleData){
            .m_pNextState = _pParameters->m_pNextState,
            .m_oSequenceElements = {
                [STEP_FADE_IN_TITLE] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = 0,
                    .OnExecute = StartFadeInTitle
                },
                [STEP_FADE_IN_POWERED_BY] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = FADE_IN_FRAMES_TITLE + INTER_FADE_FRAMES,
                    .OnExecute = StartFadeInPoweredBy
                },
                [STEP_FADE_IN_SGDK] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = FADE_IN_FRAMES_POWERED_BY + INTER_FADE_FRAMES,
                    .OnExecute = StartFadeInSGDK
                },
                [STEP_FADE_TO_WHITE_SGDK] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = FADE_IN_FRAMES_SGDK,
                    .OnExecute = StartFadeToWhiteSGDK
                },
                [STEP_FADE_FROM_WHITE_SGDK] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = FADE_TO_WHITE_FRAMES_SGDK + 10,
                    .OnExecute = StartFadeFromWhiteSGDK
                },
                [STEP_FADE_OUT] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = WAIT_FRAMES,
                    .OnExecute = StartFadeOut
                },
                [STEP_GO_TO_NEXT_STATE] = {
                    .m_pData = &_pStateTitle->m_oData,
                    .m_uDelay = FADE_OUT_FRAMES,
                    .OnExecute = GoToNextState
                }
            },
            .m_oSequence = {
                .m_pSequenceElements = _pStateTitle->m_oData.m_oSequenceElements,
                .m_uSequenceElementsCount = ARRAY_GET_LENGTH_STATIC(_pStateTitle->m_oData.m_oSequenceElements)
            },
            .m_pEngine = _pParameters->m_pEngine,
        },
        .m_oState = (StateMachineState) {
            .m_pData = &_pStateTitle->m_oData,
            .OnEnter = OnEnterTitle,
            .OnExit = OnExitTitle,
            .OnUpdate = OnUpdateTitle,
        }
    };
}

static void OnEnterTitle(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState)
{
    StateTitleData* pData = _pState->m_pData;
    ResetSequence(&pData->m_oSequence);
    SetInputManagerCallback(&pData->m_pEngine->m_oInputManager, pData, JoyHandler);
}

static void OnExitTitle(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState)
{
    StateTitleData* pData = _pState->m_pData;
    SetInputManagerCallback(&pData->m_pEngine->m_oInputManager, NULL, NULL);
}

static void OnUpdateTitle(struct StateMachine* _pStateMachine, const struct StateMachineState* _pState)
{
    StateTitleData* pData = _pState->m_pData;
    Sequence* pSequence = &pData->m_oSequence;
    if(CanEarlyExit(pData, pSequence))
        JumpToSequenceStep(pSequence, STEP_FADE_OUT, 0);
    UpdateSequence(pSequence, _pStateMachine);
}

static void StartFadeInTitle(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    StartComponentLimitFadeRange(&pData->m_pEngine->m_oPaletteManager, 1, 4, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_IN_FRAMES_TITLE);
    SND_PCM_startPlay(Taiko, sizeof(Taiko), SOUND_PCM_RATE_16000, SOUND_PAN_LEFT, false);
}

static void StartFadeInPoweredBy(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    StartComponentLimitFadeRange(&pData->m_pEngine->m_oPaletteManager, 6, 4, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_IN_FRAMES_POWERED_BY);
    SND_PCM_startPlay(Taiko, sizeof(Taiko), SOUND_PCM_RATE_16000, SOUND_PAN_RIGHT, false);
}

static void StartFadeInSGDK(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    StartComponentLimitFadeRange(&pData->m_pEngine->m_oPaletteManager, 11, 4, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_IN_FRAMES_SGDK);
    SND_PCM_startPlay(HiHat, sizeof(HiHat), SOUND_PCM_RATE_16000, SOUND_PAN_CENTER, false);
}

static void StartFadeToWhiteSGDK(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    static const PaletteLinearFadeSetting oFadeToWhiteSetting = {
        .m_oFadeColor =  VDPCOLOR_TO_RGBCOLOR_333(VDP_COLOR_WHITE),
        .m_eDirection = PALETTE_FADE_TO
    };
    StartLinearFadeRange(&pData->m_pEngine->m_oPaletteManager, 11, 4, &oFadeToWhiteSetting, FADE_TO_WHITE_FRAMES_SGDK);
}

static void StartFadeFromWhiteSGDK(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    static const PaletteLinearFadeSetting oFadeFromWhiteSetting = {
        .m_oFadeColor =  VDPCOLOR_TO_RGBCOLOR_333(VDP_COLOR_WHITE),
        .m_eDirection = PALETTE_FADE_FROM
    };
    StartLinearFadeRange(&pData->m_pEngine->m_oPaletteManager, 11, 4, &oFadeFromWhiteSetting, FADE_FROM_WHITE_FRAMES_SGDK);
}

static void StartFadeOut(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    StartComponentLimitFade(&pData->m_pEngine->m_oPaletteManager, FADE_COMPONENT_LIMIT_TO_BLACK, FADE_OUT_FRAMES);
}

static void GoToNextState(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    StateTitleData* pData = _pSequenceElement->m_pData;
    SetStateMachineState((StateMachine*)_pContextData, pData->m_pNextState);
}

static void JoyHandler(void* _pData, u16 _uJoyID, u16 _uChanged, u16 _uState)
{
    bool bSetEarlyExit = false;
    switch(_uJoyID)
    {
        case JOY_1:
            bSetEarlyExit = _uState & BUTTON_BTN;
            break;
        case JOY_2:
            bSetEarlyExit = CheckMouseButtons(_uJoyID, _uChanged, _uState, BUTTON_LMB | BUTTON_MMB | BUTTON_RMB);
        default:
            break;
    }

    if(bSetEarlyExit)
        ((StateTitleData*)_pData)->m_bIsEarlyExit = true;
}

static bool CanEarlyExit(const StateTitleData* _pData, const Sequence* _pSequence)
{
    return _pData->m_bIsEarlyExit && IsSequenceStepDone(_pSequence, STEP_FADE_FROM_WHITE_SGDK) && !IsSequenceStepDone(_pSequence, STEP_FADE_OUT);
}