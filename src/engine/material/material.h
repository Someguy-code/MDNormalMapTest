#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <engine/types/vector3.h>
#include <engine/material/materialdiffusecomponent.h>
#include <genesis.h>

struct NormalsGroup;
struct MaterialSpecularComponent;

typedef struct Material
{
    bool m_bIncludePureBlack;
    u8 m_uAlbedoColorsCount;
    const RGBColor* m_pAlbedoColors;
    u8 m_uNormalGroupsCount;
    struct NormalsGroup* m_pNormalsGroups;
    u8 m_uNormalsCount;
    const Vector3_s8* m_pNormals;
    u8 m_uTotalColorsCount;
    MaterialDiffuseComponent m_oDiffuseComponent;
    struct MaterialSpecularComponent* m_pSpecularComponent;
} Material;

struct PaletteManager;

Material CreateMaterial(const u8* _pData, u8 _uPalIndicesCount, const u8* _pPalIndices);
void DestroyMaterial(Material* _pMaterial);
Material WrapMutableMaterialExternalData(const Material* _pOriginalMaterial, u8** _pExternalData);
//Computes the colors for all palette entries assigned to _pMaterial. _pLightDirection must be a vector normalized in the [127, -128] range
void UpdateMaterial(const Material* _pMaterial, const Vector3_s8* _pLightDirection, struct PaletteManager* _pPaletteManager);

#endif //#ifndef _MATERIAL_H_