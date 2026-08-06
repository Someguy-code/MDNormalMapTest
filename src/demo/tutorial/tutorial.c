#include "tutorial.h"

#include <engine/utils/arrayutils.h>

#include <resources.h>

enum
{
    DELAY_SHOW_NAVIGATION_CONTROLS = 30,
    DELAY_SHOW_TOGGLE_TEXTURE_CONTROLS = 60,
    DELAY_SHOW_CYCLE_ILUMINATION_FEATURES_CONTROLS = 60,
    TUTORIAL_ITEMS_COUNT = ARRAY_GET_LENGTH_STATIC((Tutorial){}.m_oTutorialItems)
};

enum TutorialSteps
{
    STEP_SHOW_NAVIGATION_CONTROLS,
    STEP_SHOW_TOGGLE_TEXTURE_CONTROLS,
    STEP_SHOW_CYCLE_ILUMINATION_FEATURES_CONTROLS,
};

enum TutorialItems
{
    ITEM_NAVIGATION_CONTROLS,
    ITEM_TOGGLE_TEXTURE_CONTROLS,
    ITEM_CYCLE_ILUMINATION_FEATURES_CONTROLS
};

static u16 InitializeTutorialItems(TutorialItem _pTutorialItems[TUTORIAL_ITEMS_COUNT], struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex);
static void ShowTutorialItem(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);

u16 InitializeTutorial(Tutorial* _pTutorial, struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex)
{
    //return _uBaseTileIndex;
    (*_pTutorial) = (Tutorial){
        .m_oSequenceElements = {
            [STEP_SHOW_NAVIGATION_CONTROLS] = {
                .m_pData = &_pTutorial->m_oTutorialItems[ITEM_NAVIGATION_CONTROLS],
                .m_uDelay = DELAY_SHOW_NAVIGATION_CONTROLS,
                .OnExecute = ShowTutorialItem
            },
            [STEP_SHOW_TOGGLE_TEXTURE_CONTROLS] = {
                .m_pData = &_pTutorial->m_oTutorialItems[ITEM_TOGGLE_TEXTURE_CONTROLS],
                .m_uDelay = DELAY_SHOW_TOGGLE_TEXTURE_CONTROLS,
                .OnExecute = ShowTutorialItem
            },
            [STEP_SHOW_CYCLE_ILUMINATION_FEATURES_CONTROLS] = {
                .m_pData = &_pTutorial->m_oTutorialItems[ITEM_CYCLE_ILUMINATION_FEATURES_CONTROLS],
                .m_uDelay = DELAY_SHOW_CYCLE_ILUMINATION_FEATURES_CONTROLS,
                .OnExecute = ShowTutorialItem
            },
        },
        .m_oSequence = {
            .m_pSequenceElements = _pTutorial->m_oSequenceElements,
            .m_uSequenceElementsCount = ARRAY_GET_LENGTH_STATIC(_pTutorial->m_oSequenceElements)
        },
    };

    ResetSequence(&_pTutorial->m_oSequence);

    return InitializeTutorialItems(_pTutorial->m_oTutorialItems, _pEngine, _uPaletteIndex, _uBaseTileIndex);
}

void ReleaseTutorial(Tutorial* _pTutorial)
{
    TutorialItem* pTutorialItems = _pTutorial->m_oTutorialItems;
    for(u16 uTutorialItemIndex = 0; uTutorialItemIndex < TUTORIAL_ITEMS_COUNT; ++uTutorialItemIndex)
        ReleaseTutorialItem(&pTutorialItems[uTutorialItemIndex]);
}

void UpdateTutorial(Tutorial* _pTutorial)
{
    UpdateSequence(&_pTutorial->m_oSequence, NULL);
    TutorialItem* pTutorialItems = _pTutorial->m_oTutorialItems;
    for(u16 uTutorialItemIndex = 0; uTutorialItemIndex < TUTORIAL_ITEMS_COUNT; ++uTutorialItemIndex)
        UpdateTutorialItem(&pTutorialItems[uTutorialItemIndex]);
}

static u16 InitializeTutorialItems(TutorialItem _pTutorialItems[TUTORIAL_ITEMS_COUNT], struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex)
{
    const s16 sScreenWidthHalf = screenWidth / 2;
    const s16 sScreenHeighHalf = screenHeight / 2;
    _uBaseTileIndex = InitializeTutorialItem(&_pTutorialItems[ITEM_NAVIGATION_CONTROLS], _pEngine, &TutorialMoveLight,
        &(V2s16){sScreenWidthHalf - TutorialMoveLight.w / 2, 0}, &(V2s16){24, 0}, _uPaletteIndex, 1, 4, _uBaseTileIndex);
    const s16 sHorizontalMargin = 16;
    _uBaseTileIndex = InitializeTutorialItem(&_pTutorialItems[ITEM_TOGGLE_TEXTURE_CONTROLS], _pEngine, &TutorialToggleTexture,
        &(V2s16){sHorizontalMargin, sScreenHeighHalf - TutorialToggleTexture.h / 2}, &(V2s16){0, -24}, _uPaletteIndex, 5, 4, _uBaseTileIndex);
    _uBaseTileIndex = InitializeTutorialItem(&_pTutorialItems[ITEM_CYCLE_ILUMINATION_FEATURES_CONTROLS], _pEngine, &TutorialCycleFeatures,
        &(V2s16){screenWidth - sHorizontalMargin - TutorialCycleFeatures.w , sScreenHeighHalf - TutorialCycleFeatures.h / 2}, &(V2s16){0, 24}, _uPaletteIndex, 9, 4, _uBaseTileIndex);
    return _uBaseTileIndex;
}

static void ShowTutorialItem(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    EnableTutorialItem((TutorialItem*)_pSequenceElement->m_pData);
}