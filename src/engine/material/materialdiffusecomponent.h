#ifndef _MATERIAL_DIFFUSE_COMPONENT_H_
#define _MATERIAL_DIFFUSE_COMPONENT_H_

#include <genesis.h>
#include <engine/types/vector3.h>

typedef union
{
    const u16* m_pVDP;
    const RGBColor* m_pRGB333;
} MaterialDiffuseColorShades;

typedef struct MaterialDiffuseComponent
{
    bool m_bIsColorRGB333;
    u8 m_uFrontSurplusBits;
    u8 m_uFrontShadeMaxIndex;
    MaterialDiffuseColorShades* m_pFrontShadedAlbedoColors;
    u8 m_uBackSurplusBits;
    u8 m_uBackShadeMaxIndex;
    MaterialDiffuseColorShades* m_pBackShadedAlbedoColors;
} MaterialDiffuseComponent;

MaterialDiffuseComponent CreateMaterialDiffuseComponent(const u8* _pData, u8 _uAlbedoColorsCount, u16* _pMaterialDiffuseDataSize);
void DestroyMaterialDiffuseComponent(MaterialDiffuseComponent* _pMaterialDiffuseComponent);
u16 GetMaterialDiffuseComponentExternalDataSize(const MaterialDiffuseComponent* _pMaterialDiffuseComponent, u8 _uAlbedoColorsCount);
u8* WrapMutableMaterialDiffuseComponentExternalData(const MaterialDiffuseComponent* _pOriginalMaterialDiffuseComponent, MaterialDiffuseComponent* _pTargetMaterialDiffuseComponent, u8 _uAlbedoColorsCount, u8* _pExternalData);

#endif //#if _MATERIAL_DIFFUSE_COMPONENT_H_