#ifndef _MATERIAL_SPECULAR_COMPONENT_H_
#define _MATERIAL_SPECULAR_COMPONENT_H_

#include <genesis.h>
#include <engine/types/vector3.h>

typedef struct MaterialSpecularComponent
{
    u8 m_uFrontSurplusBits;
    u8 m_uFrontShadeMaxIndex;
    const RGBColor* m_pFrontShades;
    u8 m_uBackSurplusBits;
    u8 m_uBackShadeMaxIndex;
    const RGBColor* m_pBackShades;
} MaterialSpecularComponent;

MaterialSpecularComponent CreateMaterialSpecularComponent(const u8* _pData);
u16 GetMaterialSpecularComponentExternalDataSize(const MaterialSpecularComponent* _pMaterialSpecularComponent);
void WrapMutableMaterialSpecularComponentExternalData(const MaterialSpecularComponent* _pOriginalMaterialSpecularComponent, MaterialSpecularComponent** _pTargetMaterialSpecularComponent, u8* _pExternalData);

#endif //#if _MATERIAL_SPECULAR_COMPONENT_H_