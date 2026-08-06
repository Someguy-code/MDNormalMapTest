#ifndef _MATERIAL_MODIFIER_UI_ELEMENT_H_
#define _MATERIAL_MODIFIER_UI_ELEMENT_H_

#include <genesis.h>

#include <engine/types/Int16interpolator.h>

struct Engine;

typedef struct MaterialModifierUIElement
{
    bool m_bIsEnabled;
    bool m_bIsChecked;
    //If not available, dim the option and prevent changes
    bool m_bIsAvailable;
    u8 m_uPaletteIndex;
    u8 m_uBaseColorIndex;
    u8 m_uColorsCount;
    u16Interpolator m_oFadeInInterpolator;
    struct MaterialModifierUIElementCheck
    {
        u8 m_uBaseColorIndex;
        u8 m_uColorsCount;
        u16Interpolator m_oIntensityInterpolator;
    } m_oCheck;
    const Palette* m_pOriginalPalette;
    struct Engine* m_pEngine;
} MaterialModifierUIElement;

MaterialModifierUIElement CreateMaterialModifierUIElement(bool _bIsAvailable, u8 _uPaletteIndex, u8 _uBaseColorIndex, u8 _uColorsCount, u8 _uCheckBaseColorIndex, u8 _uCheckColorsCount, const Palette* _pOriginalPalette, struct Engine* _pEngine);
void EnableMaterialModifierUIElement(MaterialModifierUIElement* _pElement);
void UpdateMaterialModifierUIElement(MaterialModifierUIElement* _pElement, bool _bNewIsChecked);

#endif //#ifndef _MATERIAL_MODIFIER_UI_ELEMENT_H_