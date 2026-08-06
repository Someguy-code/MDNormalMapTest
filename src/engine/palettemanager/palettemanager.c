#include "palettemanager.h"

#include <engine/palettemanager/palettemanagerutils.h>

void InitializePaletteManager(PaletteManager* _pPaletteManager)
{
    *_pPaletteManager = (PaletteManager){};
    InitializePaletteManagerPostProcess(&_pPaletteManager->m_oPaletteManagerPostProcess);
    PAL_getColors(0, _pPaletteManager->m_oPaletteColors, ALL_COLORS_COUNT);
}

void UpdatePaletteManager(PaletteManager* _pPaletteManager)
{
    (void) UpdatePaletteManagerPostProcess(&_pPaletteManager->m_oPaletteManagerPostProcess, _pPaletteManager);

    const u16* pPaletteColors = _pPaletteManager->m_oPaletteColors;
    bool* pPaletteColorChangedFlags = _pPaletteManager->m_oPaletteColorChangedFlags;
    UploadPaletteChanges(pPaletteColors, pPaletteColorChangedFlags);
    memset(pPaletteColorChangedFlags, false, ALL_COLORS_COUNT * sizeof(bool));
}

void SetColorInPaletteManager(PaletteManager* _pPaletteManager, u8 _uColorIndex, u16 _uColor)
{
    _pPaletteManager->m_oPaletteColors[_uColorIndex] = _uColor;
    _pPaletteManager->m_oPaletteColorChangedFlags[_uColorIndex] = true;
}

void SetColorsInPaletteManager(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, const u16* _pColors, u16 _uColorsCount)
{
    u16* pPaletteColors = _pPaletteManager->m_oPaletteColors;
    bool* pColorChangedFlags = _pPaletteManager->m_oPaletteColorChangedFlags;
    for(u16 uColorIndex = 0; uColorIndex < _uColorsCount; ++uColorIndex)
    {
        const u16 uTargetColorIndex = _uBaseColorIndex + uColorIndex;
        pPaletteColors[uTargetColorIndex] = _pColors[uColorIndex];
        pColorChangedFlags[uTargetColorIndex] = true;
    }
}

void SetAllColorsOverPaletteManager(PaletteManager* _pPaletteManager, u16 _uColor)
{
    SetRangeColorsOverPaletteManager(_pPaletteManager, 0, ALL_COLORS_COUNT, _uColor);
}

//Sets a range of entries directly in the palette to a specific color overriding the palette manager contents
void SetRangeColorsOverPaletteManager(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, u16 _uColor)
{
    u16 oColors[ALL_COLORS_COUNT];
    memset(oColors, _uColor, _uColorsCount * sizeof(_uColor));
    PAL_setColors(_uBaseColorIndex, oColors, _uColorsCount, DMA);
    //Make sure potential palette changes are not submited to CRAM
    memset(&_pPaletteManager->m_oPaletteColorChangedFlags[_uBaseColorIndex], false, sizeof(bool) * _uColorsCount);
}

void SetPostProcessInPaletteManager(PaletteManager* _pPaletteManager, const PaletteManagerPostProcessDescriptor* _pPaletteManagerPostProcessDescriptor, const void* _pSettings, u16 _uPostProcessTime)
{
    (void)AddRangeToPaletteManagerPostProcess(&_pPaletteManager->m_oPaletteManagerPostProcess, _pSettings, 0, ALL_COLORS_COUNT, _pPaletteManagerPostProcessDescriptor, _uPostProcessTime);
}

void SetRangePostProcessInPaletteManager(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, const PaletteManagerPostProcessDescriptor* _pPaletteManagerPostProcessDescriptor, const void* _pSettings, u16 _uPostProcessTime)
{
    (void)AddRangeToPaletteManagerPostProcess(&_pPaletteManager->m_oPaletteManagerPostProcess, _pSettings, _uBaseColorIndex, _uColorsCount, _pPaletteManagerPostProcessDescriptor, _uPostProcessTime);
}