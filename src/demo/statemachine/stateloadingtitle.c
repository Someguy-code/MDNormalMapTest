#include "stateloadingtitle.h"

#include <demo/pointer/pointer.h>

#include <engine/engine.h>
#include <engine/constants/paletteconstants.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/statemachine/statemachine.h>
#include <engine/utils/colorutils.h>

#include <resources.h>

static void OnEnterLoadingTitle(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState);

void InitializeStateLoadingTitle(StateLoadingTitle* _pStateLoadingTitle, const StateLoadingTitleParameters* _pParameters)
{
    *_pStateLoadingTitle = (StateLoadingTitle) {
        .m_oData = (StateLoadingTitleData){
            .m_oCenter = *_pParameters->m_pCenter,
            .m_pNextState = _pParameters->m_pNextState,
            .m_pEngine = _pParameters->m_pEngine,
        },
        .m_oState = (StateMachineState) {
            .m_pData = &_pStateLoadingTitle->m_oData,
            .OnEnter = OnEnterLoadingTitle,
        }
    };
}

static void OnEnterLoadingTitle(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState)
{
    StateLoadingTitleData* pData = _pState->m_pData;

    const u8 oPaletteIndices[] = {PAL0};
    const Palette* pPalette = Title.palette;
    PaletteManager* pPaletteManager = &pData->m_pEngine->m_oPaletteManager;

    SetColorsInPaletteManager(pPaletteManager, oPaletteIndices[0] * PALETTE_COLORS_COUNT, pPalette->data, pPalette->length);
    SetAllColorsOverPaletteManager(pPaletteManager, 0);

    const V2u16* pCenter = &pData->m_oCenter;
    
    const TileMap* pTitleTilemap = Title.tilemap;
    const V2u16 oTitlePosition = {
        pCenter->x / 8 - pTitleTilemap->w / 2,
        pCenter->y / 8 - pTitleTilemap->h / 2
    };
    
    const u16 uFirstTileIndex = 1;
    (void)VDP_drawImageEx(BG_A, &Title, TILE_ATTR_FULL(oPaletteIndices[0], true, 0, 0, uFirstTileIndex), oTitlePosition.x, oTitlePosition.y, FALSE, DMA_QUEUE);

    SetStateMachineState(_pStateMachine, pData->m_pNextState);
}