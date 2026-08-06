#include "stateloadingmain.h"

#include <demo/materialmodifiercontroller/materialmodifiercontroller.h>
#include <demo/pointer/pointer.h>
#include <demo/tutorial/tutorial.h>

#include <engine/engine.h>
#include <engine/material/material.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/palettemanager/palettecomponentlimitfade.h>
#include <engine/statemachine/statemachine.h>

#include <resources.h>

static const u16 FADE_FRAMES = 21;

static void OnEnterLoadingMain(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState);

void InitializeStateLoadingMain(StateLoadingMain* _pStateLoadingMain, const StateLoadingMainParameters* _pParameters)
{
    *_pStateLoadingMain = (StateLoadingMain) {
        .m_oData = (StateLoadingMainData){
            .m_oCenter = *_pParameters->m_pCenter,
            .m_pNextState = _pParameters->m_pNextState,
            .m_pOutMaterial = _pParameters->m_pOutMaterial,
            .m_pOutPointer = _pParameters->m_pOutPointer,
            .m_pOutTutorial = _pParameters->m_pOutTutorial,
            .m_pOutMaterialModifierController = _pParameters->m_pOutMaterialModifierController,
            .m_pEngine = _pParameters->m_pEngine,
        },
        .m_oState = (StateMachineState) {
            .m_pData = &_pStateLoadingMain->m_oData,
            .OnEnter = OnEnterLoadingMain,
        }
    };
}

static void OnEnterLoadingMain(StateMachine* _pStateMachine, const StateMachineState* _pState, const StateMachineState* _pLastState)
{
    StateLoadingMainData* pData = _pState->m_pData;
    PaletteManager* pPaletteManager = &pData->m_pEngine->m_oPaletteManager;

    const V2u16* pCenter = &pData->m_oCenter;
    const u8 uUIPaletteIndex = PAL0;
    InitializePointer(pData->m_pOutPointer, &PointerSprite, uUIPaletteIndex, pCenter, pData->m_pEngine);
    StartComponentLimitFadeRange(pPaletteManager, uUIPaletteIndex * PALETTE_COLORS_COUNT + 1, 1, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_FRAMES);
    
    const u8 oTorusPaletteIndices[] = {PAL2, PAL3};
    Material* pMaterial = pData->m_pOutMaterial;
    *pMaterial = CreateMaterial(TorusMaterialData, 2, oTorusPaletteIndices);
    const u8 uFirstMaterialColor = oTorusPaletteIndices[0] * PALETTE_COLORS_COUNT + 1;
    const u8 uMaterialColorsRangeCount = pMaterial->m_uTotalColorsCount + pMaterial->m_uNormalGroupsCount - 1;
    StartComponentLimitFadeRange(pPaletteManager, uFirstMaterialColor, uMaterialColorsRangeCount, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_FRAMES);
    
    const TileMap* pTorusTilemap = Torus0.tilemap;
    const V2u16 oTorusPosition = {
        pCenter->x / 8 - pTorusTilemap->w / 2,
        pCenter->y / 8 - pTorusTilemap->h / 2
    };
    const u16 uFirstTileIndex = 1;
    u16 uNextTileIndex = uFirstTileIndex;
    (void)VDP_drawImageEx(BG_A, &Torus0, TILE_ATTR_FULL(oTorusPaletteIndices[0], true, 0, 0, uNextTileIndex), oTorusPosition.x, oTorusPosition.y, FALSE, DMA_QUEUE);
    uNextTileIndex += Torus0.tileset->numTile;
    (void)VDP_drawImageEx(BG_B, &Torus1, TILE_ATTR_FULL(oTorusPaletteIndices[1], true, 0, 0, uNextTileIndex), oTorusPosition.x, oTorusPosition.y, FALSE, DMA_QUEUE);
    uNextTileIndex += Torus1.tileset->numTile;

    uNextTileIndex = InitializeMaterialModifierController(pData->m_pOutMaterialModifierController, pData->m_pOutMaterial, pData->m_pEngine, uUIPaletteIndex, uNextTileIndex);

    const u8 uTutorialPaletteIndex = PAL1;
    (void)InitializeTutorial(pData->m_pOutTutorial, pData->m_pEngine, uTutorialPaletteIndex, uNextTileIndex);

    SetStateMachineState(_pStateMachine, pData->m_pNextState);
}