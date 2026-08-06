#ifndef _PALETTE_MANAGER_H_
#define _PALETTE_MANAGER_H_

#include <genesis.h>

#include <engine/constants/paletteconstants.h>
#include <engine/palettemanager/palettepostprocess.h>

typedef struct PaletteManager
{
    //Current colors for each palette entry
    u16 m_oPaletteColors[ALL_COLORS_COUNT];
    //Specifies whether the color was changed (and thus must be uploaded to CRAM by UpdatePaletteManager)
    bool m_oPaletteColorChangedFlags[ALL_COLORS_COUNT];
    //State of color post-processing (ie.: fading)
    PaletteManagerPostProcess m_oPaletteManagerPostProcess;
} PaletteManager;

void InitializePaletteManager(PaletteManager* _pPaletteManager);
void UpdatePaletteManager(PaletteManager* _pPaletteManager);
void SetColorInPaletteManager(PaletteManager* _pPaletteManager, u8 _uColorIndex, u16 _uColor);
void SetColorsInPaletteManager(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, const u16* _pColors, u16 _uColorsCount);
void SetAllColorsOverPaletteManager(PaletteManager* _pPaletteManager, u16 _uColor);
void SetRangeColorsOverPaletteManager(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, u16 _uColor);
void SetPostProcessInPaletteManager(PaletteManager* _pPaletteManager, const PaletteManagerPostProcessDescriptor* _pPaletteManagerPostProcessDescriptor, const void* _pSettings, u16 _uPostProcessTime);
void SetRangePostProcessInPaletteManager(PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, const PaletteManagerPostProcessDescriptor* _pPaletteManagerPostProcessDescriptor, const void* _pSettings, u16 _uPostProcessTime);

#endif //#ifdef _PALETTE_MANAGER_H_