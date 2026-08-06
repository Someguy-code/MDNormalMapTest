#ifndef _MATERIAL_MODIFIER_UI_H_
#define _MATERIAL_MODIFIER_UI_H_

#include <genesis.h>

#include <demo/materialmodifiercontroller/materialmodifieruielement.h>

#include <engine/sequence/sequence.h>
#include <engine/sequence/sequencestep.h>

struct Engine;

enum
{
    MATERIAL_MODIFIER_UI_ELEMENTS_COUNT = 3
};

typedef struct MaterialModifierUI
{
    MaterialModifierUIElement m_oUIElements[MATERIAL_MODIFIER_UI_ELEMENTS_COUNT];
    SequenceStep m_oSequenceItems[MATERIAL_MODIFIER_UI_ELEMENTS_COUNT];
    Sequence m_oSequence;
} MaterialModifierUI;

u16 InitializeMaterialModifierUI(MaterialModifierUI* _pMaterailaModifierUI, struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex, bool _bIsSpecularAvailable);
void UpdateMaterialModifierUI(MaterialModifierUI* _pMaterailaModifierUI, bool _bIsTextureEnabled, bool _bIsDiffuseEnabled, bool _bIsSpecularEnabled);


#endif //#ifndef _MATERIAL_MODIFIER_UI_H_