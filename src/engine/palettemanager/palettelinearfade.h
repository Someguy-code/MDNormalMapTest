#ifndef _PALETTE_LIENAR_FADE_H_
#define _PALETTE_LIENAR_FADE_H_

#include <engine/palettemanager/palettefadedirection.h>
#include <engine/types/vector3.h>
#include <genesis.h>

struct PaletteManager;

typedef struct
{
    //Fade color in RGB333
    RGBColor m_oFadeColor;
    PaletteFadeDirection m_eDirection;
} PaletteLinearFadeSetting;

void StartLinearFade(struct PaletteManager* _pPaletteManager, const PaletteLinearFadeSetting* _pSetting, u16 _uFadeTime);
void StartLinearFadeRange(struct PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, const PaletteLinearFadeSetting* _pSetting, u16 _uFadeTime);

#endif //#ifndef _PALETTE_LIENAR_FADE_H_