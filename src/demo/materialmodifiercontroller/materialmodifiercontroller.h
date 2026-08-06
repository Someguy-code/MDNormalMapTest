#ifndef _MATERIAL_MODIFIER_CONTROLLER_H_
#define _MATERIAL_MODIFIER_CONTROLLER_H_

#include <genesis.h>

#include <demo/materialmodifiercontroller/materialmodifierui.h>

#include <engine/material/mutablematerialwrapper.h>
#include <engine/types/int16interpolator.h>

struct Engine;
struct Matreial;

typedef enum
{
    NO_LIGHTING = 0b00,
    DIFFUSE_ONLY = 0b01,
    SPECULAR_ONLY = 0b10,
    DIFFUSE_AND_SPECULAR = 0b11,
    MATERIAL_FEATURES_COUNT
} MaterialFeatures;

typedef struct MaterialModifierController
{
    const Material* m_pOriginalMaterial;
    MutableMaterialWrapper m_oMutabledMaterialWrapper;
    u16Interpolator m_oTextureInterpolator;
    u16Interpolator m_oDiffuseInterpolator;
    u16Interpolator m_oSpecularInterpolator;
    bool m_bUseTexture;
    MaterialFeatures m_eMaterialFeatures;

    MaterialModifierUI m_oMaterialModifierUI;
} MaterialModifierController;

u16 InitializeMaterialModifierController(MaterialModifierController* _pMaterialModifierController, const Material* _pOriginalMaterial, struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex);
void DestroyMaterialModifierController(MaterialModifierController* _pMaterialModifierController);
bool UpdateMaterialModifierController(MaterialModifierController* _pMaterialModifierController);
void JoyHandlerMaterialModifierController(MaterialModifierController* _pMaterialModifierController, u16 _uJoyID, u16 _uChanged, u16 _uState);

#endif //#ifndef _MATERIAL_MODIFIER_CONTROLLER_H_