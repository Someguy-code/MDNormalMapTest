#include "palettecomponentlimitfade.h"

#include <engine/constants/colorconstants.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/types/vector3.h>
#include <engine/utils/arrayutils.h>
#include <engine/utils/colorutils.h>

#include <genesis.h>

//Per component brightness lower limit for each fade from white
static const RGBColor m_oWhiteComponentLimits[] = {
    {7, 7, 7}, 
    {7, 7, 6}, {7, 7, 5}, {7, 7, 4}, {7, 7, 3}, {7, 7, 2}, {7, 7, 1}, {7, 7, 0}, 
    {6, 7, 0}, {5, 7, 0}, {4, 7, 0}, {3, 7, 0}, {2, 7, 0}, {1, 7, 0}, {0, 7, 0}, 
    {0, 6, 0}, {0, 5, 0}, {0, 4, 0}, {0, 3, 0}, {0, 2, 0}, {0, 1, 0}, {0, 0, 0}, 
};

//Per component brightness upper limit for each fade from black
static const RGBColor m_oBlackComponentLimits[] = {
    {0, 0, 0}, 
    {0, 0, 1}, {0, 0, 2}, {0, 0, 3}, {0, 0, 3}, {0, 0, 5}, {0, 0, 6}, {0, 0, 7},
    {0, 1, 7}, {0, 2, 7}, {0, 3, 7}, {0, 4, 7}, {0, 5, 7}, {0, 6, 7}, {0, 7, 7},
    {1, 7, 7}, {2, 7, 7}, {3, 7, 7}, {4, 7, 7}, {5, 7, 7}, {6, 7, 7}, {7, 7, 7},
};

typedef struct
{
    u16 m_uFadeColor;
    PaletteFadeDirection m_eDirection;
    const RGBColor* m_pComponentLimits;
} PaletteLimitComponentFadeSettings;

enum {
    COMPONENT_LIMITS_COUNT = ARRAY_GET_LENGTH_STATIC(m_oWhiteComponentLimits),
};

static void ComponentLimitFade(PaletteManager* _pPaletteManager, PaletteManagerPostProcessRange* _pRange);
static u16 GetMaxComponentLimitedColor(u16 _uColor, const RGBColor* _pLimits);
static u16 GetMinComponentLimitedColor(u16 _uColor, const RGBColor* _pLimits);

static const PaletteManagerPostProcessDescriptor m_oPaletteComponentLimitFade = {
    .OnPalettePostProcess = ComponentLimitFade,
    .m_uMaxValue = COMPONENT_LIMITS_COUNT - 1
};

static const PaletteLimitComponentFadeSettings m_oPaletteLimitComponentFadeSettings[] =
{
    [FADE_COMPONENT_LIMIT_TO_BLACK] = {
        .m_uFadeColor = VDP_COLOR_BLACK,
        .m_eDirection = PALETTE_FADE_TO,
        .m_pComponentLimits = m_oBlackComponentLimits,
    },
    [FADE_COMPONENT_LIMIT_FROM_BLACK] = {
        .m_uFadeColor = VDP_COLOR_BLACK,
        .m_eDirection = PALETTE_FADE_FROM,
        .m_pComponentLimits = m_oBlackComponentLimits,
    },
    [FADE_COMPONENT_LIMIT_TO_WHITE] = {
        .m_uFadeColor = VDP_COLOR_WHITE,
        .m_eDirection = PALETTE_FADE_TO,
        .m_pComponentLimits = m_oWhiteComponentLimits,
    },
    [FADE_COMPONENT_LIMIT_FROM_WHITE] = {
        .m_uFadeColor = VDP_COLOR_WHITE,
        .m_eDirection = PALETTE_FADE_FROM,
        .m_pComponentLimits = m_oWhiteComponentLimits,
    },
};

void StartComponentLimitFade(struct PaletteManager* _pPaletteManager, PaletteLimitComponentFadeType _eFadeType, u16 _uFadeTime)
{
    StartComponentLimitFadeRange(_pPaletteManager, 0, ALL_COLORS_COUNT, _eFadeType, _uFadeTime);
}

void StartComponentLimitFadeRange(struct PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, PaletteLimitComponentFadeType _eFadeType, u16 _uFadeTime)
{
    const PaletteLimitComponentFadeSettings* pSettings = &m_oPaletteLimitComponentFadeSettings[_eFadeType];
    if(pSettings->m_eDirection == PALETTE_FADE_FROM)
        SetRangeColorsOverPaletteManager(_pPaletteManager, _uBaseColorIndex, _uColorsCount, pSettings->m_uFadeColor);
    SetRangePostProcessInPaletteManager(_pPaletteManager, _uBaseColorIndex, _uColorsCount, &m_oPaletteComponentLimitFade, pSettings, _uFadeTime);
}

static void ComponentLimitFade(PaletteManager* _pPaletteManager, PaletteManagerPostProcessRange* _pRange)
{
    const PaletteLimitComponentFadeSettings* pSettings = _pRange->m_pSetting;
    u16 uFadeLevel = UpdateU16Interpolator(&_pRange->m_oPostProcessValueInterpolator);
    uFadeLevel = pSettings->m_eDirection == PALETTE_FADE_TO ? COMPONENT_LIMITS_COUNT - 1 - uFadeLevel : uFadeLevel;
    const u16* pPalette = _pPaletteManager->m_oPaletteColors;
    PaletteManagerPostProcess* pPaletteManagerPostProcess = &_pPaletteManager->m_oPaletteManagerPostProcess;
    u16* pPalettePostProcess = pPaletteManagerPostProcess->m_oPaletteColorsPostProcess;
    const u8 uBaseColorIndex = _pRange->m_uFirstColorIndex;
    const u8 uColorIndexSentinel = uBaseColorIndex + _pRange->m_uColorsCount;
    switch(pSettings->m_uFadeColor)
    {
        case VDP_COLOR_WHITE:
            {
                const RGBColor* pLimits = &m_oWhiteComponentLimits[uFadeLevel];
                for(u16 uColorIndex = uBaseColorIndex; uColorIndex < uColorIndexSentinel; ++uColorIndex)
                    pPalettePostProcess[uColorIndex] = GetMaxComponentLimitedColor(pPalette[uColorIndex], pLimits);
            }
            break;
        case VDP_COLOR_BLACK:
            {
                const RGBColor* pLimits = &m_oBlackComponentLimits[uFadeLevel];
                for(u16 uColorIndex = uBaseColorIndex; uColorIndex < uColorIndexSentinel; ++uColorIndex)
                    pPalettePostProcess[uColorIndex] = GetMinComponentLimitedColor(pPalette[uColorIndex], pLimits);
            }
            break;
        default:
            break;
    }
}

static u16 GetMaxComponentLimitedColor(u16 _uColor, const RGBColor* _pLimits)
{
    const u8 uR = GET_VDPCOLOR_R(_uColor);
    const u8 uG = GET_VDPCOLOR_G(_uColor);
    const u8 uB = GET_VDPCOLOR_B(_uColor);
    return RGB3_3_3_TO_VDPCOLOR(max(uR, _pLimits->m_uR), max(uG, _pLimits->m_uG), max(uB, _pLimits->m_uB));
}

static u16 GetMinComponentLimitedColor(u16 _uColor, const RGBColor* _pLimits)
{
    const u8 uR = GET_VDPCOLOR_R(_uColor);
    const u8 uG = GET_VDPCOLOR_G(_uColor);
    const u8 uB = GET_VDPCOLOR_B(_uColor);
    return RGB3_3_3_TO_VDPCOLOR(min(uR, _pLimits->m_uR), min(uG, _pLimits->m_uG), min(uB, _pLimits->m_uB));
}