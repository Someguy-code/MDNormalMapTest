#ifndef _PALETTE_COMPONENT_LIMIT_FADE_H_
#define _PALETTE_COMPONENT_LIMIT_FADE_H_

#include <engine/palettemanager/palettefadedirection.h>

#include <genesis.h>

struct PaletteManager;

typedef enum
{
    FADE_COMPONENT_LIMIT_TO_BLACK,
    FADE_COMPONENT_LIMIT_FROM_BLACK,
    FADE_COMPONENT_LIMIT_TO_WHITE,
    FADE_COMPONENT_LIMIT_FROM_WHITE,
} PaletteLimitComponentFadeType;

void StartComponentLimitFade(struct PaletteManager* _pPaletteManager, PaletteLimitComponentFadeType _eFadeType, u16 _uFadeTime);
void StartComponentLimitFadeRange(struct PaletteManager* _pPaletteManager, u8 _uBaseColorIndex, u8 _uColorsCount, PaletteLimitComponentFadeType _eFadeType, u16 _uFadeTime);

#endif //#ifndef _PALETTE_COMPONENT_LIMIT_FADE_H_