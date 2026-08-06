#include "materialmodifierui.h"

#include <engine/engine.h>
#include <engine/constants/colorconstants.h>

#include <resources.h>

enum MaterialModifierUIConstants
{
    TEXTURE_CHECKBOX_DELAY = 30,
    DIFFUSE_CHECKBOX_DELAY = 16,
    SPECULAR_CHECKBOX_DELAY = 16,
};

enum MaterialModifierUIElementID
{
    MATERIAL_MODIFIER_TEXTURE_CHECKBOX,
    MATERIAL_MODIFIER_DIFFUSE_CHECKBOX,
    MATERIAL_MODIFIER_SPECULAR_CHECKBOX
};

static void FadeInUIElement(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData);

u16 InitializeMaterialModifierUI(MaterialModifierUI* _pMaterailaModifierUI, Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex, bool _bIsSpecularAvailable)
{
    Palette* pPalette = MaterialModifierUIImage.palette;

    PaletteManager* pPaletteManager = &_pEngine->m_oPaletteManager;
    const u8 uFirstColorIndex = 2;
    const u8 uColorsRangeCount = pPalette->length - uFirstColorIndex;
    SetColorsInPaletteManager(pPaletteManager, _uPaletteIndex * PALETTE_COLORS_COUNT + uFirstColorIndex, &pPalette->data[uFirstColorIndex], uColorsRangeCount);

    MaterialModifierUIElement* pUIElements = _pMaterailaModifierUI->m_oUIElements;
    const u8 uBaseColorIndex = _uPaletteIndex * PALETTE_COLORS_COUNT;
    *_pMaterailaModifierUI = (MaterialModifierUI){
        .m_oUIElements = {
            [MATERIAL_MODIFIER_TEXTURE_CHECKBOX] = CreateMaterialModifierUIElement(true, _uPaletteIndex, uBaseColorIndex + 2, 2, uBaseColorIndex + 8, 1, pPalette, _pEngine),
            [MATERIAL_MODIFIER_DIFFUSE_CHECKBOX] = CreateMaterialModifierUIElement(true, _uPaletteIndex, uBaseColorIndex + 4, 2, uBaseColorIndex + 9, 1, pPalette, _pEngine),
            [MATERIAL_MODIFIER_SPECULAR_CHECKBOX] = CreateMaterialModifierUIElement(_bIsSpecularAvailable, _uPaletteIndex, uBaseColorIndex + 6, 2, uBaseColorIndex + 10, 1, pPalette, _pEngine),
        },
        .m_oSequenceItems = {
            [MATERIAL_MODIFIER_TEXTURE_CHECKBOX] = {
                .m_pData = &pUIElements[MATERIAL_MODIFIER_TEXTURE_CHECKBOX],
                .m_uDelay = TEXTURE_CHECKBOX_DELAY,
                .OnExecute = FadeInUIElement
            },
            [MATERIAL_MODIFIER_DIFFUSE_CHECKBOX] = {
                .m_pData = &pUIElements[MATERIAL_MODIFIER_DIFFUSE_CHECKBOX],
                .m_uDelay = DIFFUSE_CHECKBOX_DELAY,
                .OnExecute = FadeInUIElement
            },
            [MATERIAL_MODIFIER_SPECULAR_CHECKBOX] = {
                .m_pData = &pUIElements[MATERIAL_MODIFIER_SPECULAR_CHECKBOX],
                .m_uDelay = SPECULAR_CHECKBOX_DELAY,
                .OnExecute = FadeInUIElement
            },
        },
        .m_oSequence = {
            .m_pSequenceElements = _pMaterailaModifierUI->m_oSequenceItems,
            .m_uSequenceElementsCount = MATERIAL_MODIFIER_UI_ELEMENTS_COUNT
        }
    };

    ResetSequence(&_pMaterailaModifierUI->m_oSequence);

    V2s16 oUIPosition = {1, screenHeight/8 - MaterialModifierUIImage.tilemap->h};
    (void)VDP_drawImageEx(BG_A, &MaterialModifierUIImage, TILE_ATTR_FULL(_uPaletteIndex, true, 0, 0, _uBaseTileIndex), oUIPosition.x, oUIPosition.y, FALSE, DMA_QUEUE);

    return _uBaseTileIndex + MaterialModifierUIImage.tileset->numTile;
}

void UpdateMaterialModifierUI(MaterialModifierUI* _pMaterailaModifierUI, bool _bIsTextureEnabled, bool _bIsDiffuseEnabled, bool _bIsSpecularEnabled)
{
    UpdateSequence(&_pMaterailaModifierUI->m_oSequence, NULL);
    MaterialModifierUIElement* pUIElements = _pMaterailaModifierUI->m_oUIElements;
    UpdateMaterialModifierUIElement(&pUIElements[MATERIAL_MODIFIER_TEXTURE_CHECKBOX], _bIsTextureEnabled);
    UpdateMaterialModifierUIElement(&pUIElements[MATERIAL_MODIFIER_DIFFUSE_CHECKBOX], _bIsDiffuseEnabled);
    UpdateMaterialModifierUIElement(&pUIElements[MATERIAL_MODIFIER_SPECULAR_CHECKBOX], _bIsSpecularEnabled);
}

static void FadeInUIElement(struct Sequence* _pStateSequence, const struct SequenceStep* _pSequenceElement, void* _pContextData)
{
    MaterialModifierUIElement* pUIElement = _pSequenceElement->m_pData;
    EnableMaterialModifierUIElement(pUIElement);
}