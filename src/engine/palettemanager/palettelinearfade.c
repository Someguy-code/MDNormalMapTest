#include "palettelinearfade.h"

#include <engine/palettemanager/palettemanager.h>
#include <engine/palettemanager/palettepostprocess.h>
#include <engine/utils/colorutils.h>

static void LinearFade(struct PaletteManager* _pPaletteManager, struct PaletteManagerPostProcessRange* _pRange);
static u16 GetInterpolatedColor(u16 _uBaseColor, const RGBColor* _pFadeColor, u8 _uBalance);

enum {
    INTERPOLATION_LEVELS_COUNT = 8 //Maximum number of linear interpolation levels between any 2 given colors (due to 3 bit per component)
};

static const PaletteManagerPostProcessDescriptor m_oPaletteLinearFade = {
    .OnPalettePostProcess = LinearFade,
    .m_uMaxValue = INTERPOLATION_LEVELS_COUNT - 1
};

void StartLinearFade(PaletteManager* _pPaletteManager, const PaletteLinearFadeSetting* _pSetting, u16 _uFadeTime)
{
    StartLinearFadeRange(_pPaletteManager, 0, ALL_COLORS_COUNT, _pSetting, _uFadeTime);
}

void StartLinearFadeRange(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, const PaletteLinearFadeSetting* _pSetting, u16 _uFadeTime)
{
    if(_pSetting->m_eDirection == PALETTE_FADE_FROM)
        SetRangeColorsOverPaletteManager(_pPaletteManager, _uBaseColorIndex, _uColorsCount, RGBCOLOR_333_TO_VDPCOLOR(_pSetting->m_oFadeColor));
    SetRangePostProcessInPaletteManager(_pPaletteManager, _uBaseColorIndex, _uColorsCount, &m_oPaletteLinearFade, _pSetting, _uFadeTime);
}   

static void LinearFade( PaletteManager* _pPaletteManager, struct PaletteManagerPostProcessRange* _pRange)
{
    const PaletteLinearFadeSetting* pSettings = _pRange->m_pSetting;
    u16 uFadeLevel = UpdateU16Interpolator(&_pRange->m_oPostProcessValueInterpolator);
    uFadeLevel = pSettings->m_eDirection == PALETTE_FADE_FROM ? INTERPOLATION_LEVELS_COUNT - 1 - uFadeLevel : uFadeLevel;
    const u16* pPalette = _pPaletteManager->m_oPaletteColors;
    PaletteManagerPostProcess* pPaletteManagerPostProcess = &_pPaletteManager->m_oPaletteManagerPostProcess;
    u16* pPalettePostProcess = pPaletteManagerPostProcess->m_oPaletteColorsPostProcess;
    const u8 uBaseColorIndex = _pRange->m_uFirstColorIndex;
    const u8 uColorIndexSentinel = uBaseColorIndex + _pRange->m_uColorsCount;
    for(u16 uColorIndex = uBaseColorIndex; uColorIndex < uColorIndexSentinel; ++uColorIndex)
        pPalettePostProcess[uColorIndex] = GetInterpolatedColor(pPalette[uColorIndex], &pSettings->m_oFadeColor, uFadeLevel);
}

static u16 GetInterpolatedColor(u16 _uBaseColor, const RGBColor* _pFadeColor, u8 _uBalance)
{
    const u8 uR = GET_VDPCOLOR_R(_uBaseColor);
    const u8 uG = GET_VDPCOLOR_G(_uBaseColor);
    const u8 uB = GET_VDPCOLOR_B(_uBaseColor);

    return RGB333_LERP(uR, uG, uB, _pFadeColor->m_uR, _pFadeColor->m_uG, _pFadeColor->m_uB, _uBalance);
}