#include "tutorialitem.h"

#include <engine/engine.h>
#include <engine/constants/colorconstants.h>
#include <engine/constants/paletteconstants.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/palettemanager/palettecomponentlimitfade.h>
#include <engine/utils/arrayutils.h>

enum
{
    FADE_IN_FRAMES = 42,
    FADE_OUT_FRAMES = 42,
    MOVEMENT_FRAMES = 60,
    STAY_FRAMES = 5 * 60
};

enum SequenceSteps
{
    STEP_MOVE_FADE_OUT,
    STEP_FADE_OUT,
    STEP_DISABLE
};

static void MoveFadeOut(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void FadeOut(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void Disable(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);
static void OnSingleFrameChanged(Sprite* sprite);

u16 InitializeTutorialItem(TutorialItem* _pTutorialItem, Engine* _pEngine, const SpriteDefinition* _pSpriteDefinition, const V2s16* _pTargetPosition, const V2s16* _pFadePositionDelta, u8 _uPaletteIndex, u8 _uBaseColorIndex, u8 _uColorsCount, u16 _uBaseTileIndex)
{
    const s16 sStartPositionX = _pTargetPosition->x - _pFadePositionDelta->x;
    const s16 sStartPositionY = _pTargetPosition->y - _pFadePositionDelta->y;
    //Assume the sprite has onely one animation with one frame. No need to keep the frames table around
    u16 uSpriteTilesCount = 0;
    u16** pSpriteFrames = SPR_loadAllFrames(_pSpriteDefinition, _uBaseTileIndex, &uSpriteTilesCount);
    MEM_free(pSpriteFrames);

    Sprite* pSprite = SPR_addSpriteExSafe(_pSpriteDefinition, sStartPositionX, sStartPositionY, TILE_ATTR_FULL(_uPaletteIndex, TRUE, FALSE, FALSE, _uBaseTileIndex), 0);
    pSprite->data = _uBaseTileIndex;
    SPR_setFrameChangeCallback(pSprite, &OnSingleFrameChanged);
    SPR_setVisibility(pSprite, HIDDEN);

    PaletteManager* pPaletteManager = &_pEngine->m_oPaletteManager;
    const u16 uAbsoluteBaseColorIndex = PALETTE_COLORS_COUNT * _uPaletteIndex + _uBaseColorIndex;
    SetColorsInPaletteManager(pPaletteManager, uAbsoluteBaseColorIndex, &_pSpriteDefinition->palette->data[_uBaseColorIndex], _uColorsCount);
    SetRangeColorsOverPaletteManager(pPaletteManager, uAbsoluteBaseColorIndex, _uColorsCount, VDP_COLOR_BLACK);

    *_pTutorialItem = (TutorialItem){
        .m_bIsEnabled = false,
        .m_pSprite = pSprite,
        .m_oTargetPosition = *_pTargetPosition,
        .m_oFadePositionDelta = *_pFadePositionDelta,
        .m_uAbsoluteBaseColorIndex = uAbsoluteBaseColorIndex,
        .m_uColorsCount = _uColorsCount,
        .m_oSequenceElements = {
            [STEP_MOVE_FADE_OUT] = {
                .m_pData = _pTutorialItem,
                .m_uDelay = STAY_FRAMES,
                .OnExecute = MoveFadeOut
            },
            [STEP_FADE_OUT] = {
                .m_pData = _pTutorialItem,
                .m_uDelay = MOVEMENT_FRAMES - FADE_OUT_FRAMES,
                .OnExecute = FadeOut
            },
            [STEP_DISABLE] = {
                .m_pData = _pTutorialItem,
                .m_uDelay = FADE_OUT_FRAMES,
                .OnExecute = Disable
            }
        },
        .m_oSequence = {
            .m_pSequenceElements = _pTutorialItem->m_oSequenceElements,
            .m_uSequenceElementsCount = ARRAY_GET_LENGTH_STATIC(_pTutorialItem->m_oSequenceElements)
        },
        .m_pEngine = _pEngine,
    };

    InitializeS16Interpolator(&_pTutorialItem->m_oXInterpolator, sStartPositionX, _pTargetPosition->x, 
        _pFadePositionDelta->x == 0 ? 0 : MOVEMENT_FRAMES);
    InitializeS16Interpolator(&_pTutorialItem->m_oYInterpolator, sStartPositionY, _pTargetPosition->y, 
        _pFadePositionDelta->y == 0 ? 0 : MOVEMENT_FRAMES);

    ResetSequence(&_pTutorialItem->m_oSequence);

    return _uBaseTileIndex + uSpriteTilesCount;
}

void ReleaseTutorialItem(TutorialItem* _pTutorialItem)
{
    SPR_releaseSprite(_pTutorialItem->m_pSprite);
}

void EnableTutorialItem(TutorialItem* _pTutorialItem)
{
    _pTutorialItem->m_bIsEnabled = true;
    SPR_setVisibility(_pTutorialItem->m_pSprite, VISIBLE);
    StartComponentLimitFadeRange(&_pTutorialItem->m_pEngine->m_oPaletteManager,
        _pTutorialItem->m_uAbsoluteBaseColorIndex, _pTutorialItem->m_uColorsCount, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_IN_FRAMES);
}

void UpdateTutorialItem(TutorialItem* _pTutorialItem)
{
    if(!_pTutorialItem->m_bIsEnabled)
        return;

    const s16 sX = UpdateS16Interpolator(&_pTutorialItem->m_oXInterpolator);
    const s16 sY = UpdateS16Interpolator(&_pTutorialItem->m_oYInterpolator);

    SPR_setPosition(_pTutorialItem->m_pSprite, sX, sY);

    UpdateSequence(&_pTutorialItem->m_oSequence, NULL);
}

static void MoveFadeOut(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    TutorialItem* pTutorialItem = _pSequenceElement->m_pData;
    const V2s16* pTargetPosition = &pTutorialItem->m_oTargetPosition;
    const V2s16* pFadePositionDelta = &pTutorialItem->m_oFadePositionDelta;
    const s16 sEndPositionX = pTargetPosition->x + pFadePositionDelta->x;
    const s16 sEndPositionY = pTargetPosition->y + pFadePositionDelta->y;
    InitializeS16Interpolator(&pTutorialItem->m_oXInterpolator, pTargetPosition->x, sEndPositionX, 
        pFadePositionDelta->x == 0 ? 0 : MOVEMENT_FRAMES);
    InitializeS16Interpolator(&pTutorialItem->m_oYInterpolator, pTargetPosition->y, sEndPositionY, 
        pFadePositionDelta->y == 0 ? 0 : MOVEMENT_FRAMES);
}

static void FadeOut(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    TutorialItem* pTutorialItem = _pSequenceElement->m_pData;
    StartComponentLimitFadeRange(&pTutorialItem->m_pEngine->m_oPaletteManager,
        pTutorialItem->m_uAbsoluteBaseColorIndex, pTutorialItem->m_uColorsCount, FADE_COMPONENT_LIMIT_TO_BLACK, FADE_OUT_FRAMES);
}

static void Disable(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    TutorialItem* pTutorialItem = _pSequenceElement->m_pData;
    pTutorialItem->m_bIsEnabled = false;
    SPR_setVisibility(pTutorialItem->m_pSprite, HIDDEN);
}

static void OnSingleFrameChanged(Sprite* _pSprite)
{
    SPR_setVRAMTileIndex(_pSprite, (u16)_pSprite->data);
}