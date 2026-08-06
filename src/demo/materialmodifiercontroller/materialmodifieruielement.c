#include "materialmodifieruielement.h"

#include <engine/engine.h>
#include <engine/constants/colorconstants.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/palettemanager/palettecomponentlimitfade.h>
#include <engine/utils/colorutils.h>
#include <engine/types/vector3.h>

enum MaterialModifierUIElementConstants
{
    FADE_IN_FRAMES_TITLE = 9,
    CHECK_INTERPOLATION_FRAMES = 7
};

static void SetUnavailableColors(u8 _uPaletteIndex, u8 _uBaseColorIndex, u8 _uColorsCount, u8 _uCheckBaseColorIndex, u8 _uCheckColorsCount, const Palette* _pOriginalPalette, PaletteManager* _pPaletteManager);
static u8 GetCheckIntesity(bool _bIsChecked);

MaterialModifierUIElement CreateMaterialModifierUIElement(bool _bIsAvailable, u8 _uPaletteIndex, u8 _uBaseColorIndex, u8 _uColorsCount, u8 _uCheckBaseColorIndex, u8 _uCheckColorsCount, const Palette* _pOriginalPalette, struct Engine* _pEngine)
{
    PaletteManager* pPaletteManager = &_pEngine->m_oPaletteManager;

    if(!_bIsAvailable)
        SetUnavailableColors(_uPaletteIndex, _uBaseColorIndex, _uColorsCount, _uCheckBaseColorIndex, _uCheckColorsCount, _pOriginalPalette, pPaletteManager);

    SetRangeColorsOverPaletteManager(pPaletteManager, _uBaseColorIndex, _uColorsCount, VDP_COLOR_BLACK);
    SetRangeColorsOverPaletteManager(pPaletteManager, _uCheckBaseColorIndex, _uCheckColorsCount, VDP_COLOR_BLACK);

    return (MaterialModifierUIElement){
        .m_bIsEnabled = false,
        .m_bIsChecked = _bIsAvailable,
        .m_bIsAvailable = _bIsAvailable,
        .m_uPaletteIndex = _uPaletteIndex,
        .m_uBaseColorIndex = _uBaseColorIndex,
        .m_uColorsCount = _uColorsCount,
        .m_oCheck = {
            .m_uBaseColorIndex = _uCheckBaseColorIndex,
            .m_uColorsCount = _uCheckColorsCount
        },
        .m_pOriginalPalette = _pOriginalPalette,
        .m_pEngine = _pEngine
    };
}

void EnableMaterialModifierUIElement(MaterialModifierUIElement* _pElement)
{
    _pElement->m_bIsEnabled = _pElement->m_bIsAvailable;
    PaletteManager* pPaletteManager = &_pElement->m_pEngine->m_oPaletteManager;
    StartComponentLimitFadeRange(pPaletteManager, _pElement->m_uBaseColorIndex, _pElement->m_uColorsCount, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_IN_FRAMES_TITLE);
    const struct MaterialModifierUIElementCheck* pCheck = &_pElement->m_oCheck;
    StartComponentLimitFadeRange(pPaletteManager, pCheck->m_uBaseColorIndex, pCheck->m_uColorsCount, FADE_COMPONENT_LIMIT_FROM_BLACK, FADE_IN_FRAMES_TITLE);
}

void UpdateMaterialModifierUIElement(MaterialModifierUIElement* _pElement, bool _bNewIsChecked)
{
    if(!_pElement->m_bIsEnabled)
        return;

    struct MaterialModifierUIElementCheck* pCheck = &_pElement->m_oCheck;
    u16Interpolator* pIntensityInterpolator = &pCheck->m_oIntensityInterpolator;
    bool bIsInterpolatorInitialized = IsU16InterpolatorInitialized(pIntensityInterpolator);
    if(!bIsInterpolatorInitialized)
    {
        if(!_bNewIsChecked)
        {
            InitializeU16Interpolator(pIntensityInterpolator, MAX_COLOR_COMPONENT, 0, CHECK_INTERPOLATION_FRAMES);
            bIsInterpolatorInitialized = true;
        }
    }
    else if(_bNewIsChecked != _pElement->m_bIsChecked)
        FlipU16Interpolator(pIntensityInterpolator, GetCheckIntesity(_bNewIsChecked), CHECK_INTERPOLATION_FRAMES);

    if(bIsInterpolatorInitialized && !IsU16InterpolatorFinished(pIntensityInterpolator))
    {
        const u8 uCheckIntensity = UpdateU16Interpolator(pIntensityInterpolator);
        const u8 uBaseColorIndex = pCheck->m_uBaseColorIndex;
        const u8 uColorSentinel = uBaseColorIndex + pCheck->m_uColorsCount;
        PaletteManager* pPaletteManager = &_pElement->m_pEngine->m_oPaletteManager;
        for(u8 uColorIndex = uBaseColorIndex; uColorIndex < uColorSentinel; ++uColorIndex)
        {
            const u16 uOriginalColor = _pElement->m_pOriginalPalette->data[uColorIndex];
            SetColorInPaletteManager(pPaletteManager, uColorIndex, VDPCOLOR_LERP(VDP_COLOR_BLACK, uOriginalColor, uCheckIntensity));
        }
    }
    
    _pElement->m_bIsChecked = _bNewIsChecked;
}

static void SetUnavailableColors(u8 _uPaletteIndex, u8 _uBaseColorIndex, u8 _uColorsCount, u8 _uCheckBaseColorIndex, u8 _uCheckColorsCount, const Palette* _pOriginalPalette, PaletteManager* _pPaletteManager)
{
    const u8 uBasePaletteIndex = _uPaletteIndex * PALETTE_COLORS_COUNT;

    //Dim option colors
    const u8 uColorIndexSentinel = _uBaseColorIndex + _uColorsCount;
    for(u8 uColorIndex = _uBaseColorIndex; uColorIndex < uColorIndexSentinel; ++uColorIndex)
    {
        const u16 uOriginalColor = _pOriginalPalette->data[uColorIndex - uBasePaletteIndex];
        SetColorInPaletteManager(_pPaletteManager, uColorIndex, VDPCOLOR_LERP(VDP_COLOR_BLACK, uOriginalColor, 4));
    }

    //Hide check
    const u8 uCheckColorIndexSentinel = _uCheckBaseColorIndex + _uCheckColorsCount;
    for(u8 uColorIndex = _uCheckBaseColorIndex; uColorIndex < uCheckColorIndexSentinel; ++uColorIndex)
        SetColorInPaletteManager(_pPaletteManager, uColorIndex, VDP_COLOR_BLACK);
}

[[gnu::pure]] static u8 GetCheckIntesity(bool _bIsChecked)
{
    return _bIsChecked ? MAX_COLOR_COMPONENT : 0;
}