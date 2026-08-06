#include "material.h"

#include <engine/material/materialspecularcomponent.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/utils/mathutils.h>
#include <engine/utils/mdprofiler.h>

//Range of contiguous palette entries used by a material. Must be palette-relative on serialized data and absolute in real time
typedef struct NormalsGroup
{
    u8 m_uFirstIndex;
    u8 m_uLastIndex;
} NormalsGroup;

static u16 GetMaterialExternalDataSize(const Material* _pMaterial);
static Vector3_s8 GetHalfVector(const Vector3_s8* _pLightDirection);
static u8 GetLightStrength(const Vector3_s8* _pNormal, const Vector3_s8* _pLightDirection, bool* _pIsFrontFacing);
static u8 GetShadeIndex(u8 _uLightStrength, u8 _uDiffuseSurplusBitsCount, u8 _uDiffuseComponentMax);
static u16 GetDiffuseShadedColor(MaterialDiffuseColorShades _pShades, u8 _uDiffuseShadeIndex);
static u16 GetDiffuseAndSpecularShadedColor(MaterialDiffuseColorShades _pShades, u8 _uDiffuseShadeIndex, const RGBColor* _pSpecularColor);

Material CreateMaterial(const u8* _pData, u8 _uPalIndicesCount, const u8* _pPalIndices)
{
    const bool bIncludePureBlack = _pData[0];
    
    const u8 uAlbedoColorsCount = _pData[1];
    const RGBColor* pAlbedoColors = (const RGBColor*)(&_pData[2]);

    const u8* pNormalsGroupsData = (const u8*)(pAlbedoColors + uAlbedoColorsCount);
    const u8 uNormalsGroupsCount = *pNormalsGroupsData;
    assert(uNormalsGroupsCount == _uPalIndicesCount);
    const NormalsGroup* pNextNormalsGroupData = (const NormalsGroup*)&pNormalsGroupsData[1];
    NormalsGroup* pNormalsGroups = (NormalsGroup*)MEM_alloc(uNormalsGroupsCount * sizeof(NormalsGroup));
    for(u8 uNormalsGroupIndex = 0; uNormalsGroupIndex < uNormalsGroupsCount; ++uNormalsGroupIndex)
    {
        const u8 uBasePalIndex = _pPalIndices[uNormalsGroupIndex] << 4;
        pNormalsGroups[uNormalsGroupIndex] = (NormalsGroup){
            .m_uFirstIndex = uBasePalIndex + pNextNormalsGroupData->m_uFirstIndex,
            .m_uLastIndex = uBasePalIndex + pNextNormalsGroupData->m_uLastIndex
        };
        ++pNextNormalsGroupData;
    }
    
    const u8* pNormalsData = (const u8*)pNextNormalsGroupData;
    const u8 uNormalsCount = *pNormalsData;
    const Vector3_s8* pNormals = (const Vector3_s8*)&pNormalsData[1];

    u16 uDiffuseComponentSize = 0;
    const u8* pDiffuseComponentData = pNormalsData + 1 + 3 * uNormalsCount;
    MaterialDiffuseComponent oDiffuseComponent = CreateMaterialDiffuseComponent(pDiffuseComponentData, uAlbedoColorsCount, &uDiffuseComponentSize);

    MaterialSpecularComponent* pSpecularComponent = NULL;
    const bool bHasSpecular = *(pDiffuseComponentData + uDiffuseComponentSize);
    if(bHasSpecular)
    {
        pSpecularComponent = (MaterialSpecularComponent*)MEM_alloc(sizeof(MaterialSpecularComponent));
        *pSpecularComponent = CreateMaterialSpecularComponent(pDiffuseComponentData + uDiffuseComponentSize + 1);
    }

    return (Material){
        .m_bIncludePureBlack = bIncludePureBlack,
        .m_uAlbedoColorsCount = uAlbedoColorsCount,
        .m_pAlbedoColors = pAlbedoColors,
        .m_uNormalGroupsCount = uNormalsGroupsCount,
        .m_pNormalsGroups = pNormalsGroups,
        .m_uNormalsCount = uNormalsCount,
        .m_pNormals = pNormals,
        .m_uTotalColorsCount = uNormalsCount * uAlbedoColorsCount + (bIncludePureBlack ? 1 : 0),
        .m_oDiffuseComponent = oDiffuseComponent,
        .m_pSpecularComponent = pSpecularComponent
    };
}

void DestroyMaterial(Material* _pMaterial)
{
    MEM_free(_pMaterial->m_pNormalsGroups);
    DestroyMaterialDiffuseComponent(&_pMaterial->m_oDiffuseComponent);
    MEM_free(_pMaterial->m_pSpecularComponent);
}

Material WrapMutableMaterialExternalData(const Material* _pOriginalMaterial, u8** _pExternalData)
{
    Material oMaterial = *_pOriginalMaterial;

    const u16 uExternalDataSize = GetMaterialExternalDataSize(_pOriginalMaterial);
    *_pExternalData = MEM_alloc(uExternalDataSize);

    const u16 uNormalsGroupSize = _pOriginalMaterial->m_uNormalGroupsCount * sizeof(NormalsGroup);
    NormalsGroup* pNormalsGroups = (NormalsGroup*)MEM_alloc(uNormalsGroupSize);
    memcpy(pNormalsGroups, _pOriginalMaterial->m_pNormalsGroups, uNormalsGroupSize);
    oMaterial.m_pNormalsGroups = pNormalsGroups;

    const u16 uNormalsSize = sizeof(Vector3_s8) * _pOriginalMaterial->m_uNormalsCount;
    memcpy(*_pExternalData, _pOriginalMaterial->m_pNormals, uNormalsSize);
    oMaterial.m_pNormals = (Vector3_s8*)*_pExternalData;

    u8* pTargetDiffuseExternalData = (*_pExternalData) + uNormalsSize;
    u8* pTargetSpecularExternalData = WrapMutableMaterialDiffuseComponentExternalData(&_pOriginalMaterial->m_oDiffuseComponent, &oMaterial.m_oDiffuseComponent, _pOriginalMaterial->m_uAlbedoColorsCount, pTargetDiffuseExternalData);
    WrapMutableMaterialSpecularComponentExternalData(_pOriginalMaterial->m_pSpecularComponent, &oMaterial.m_pSpecularComponent, pTargetSpecularExternalData);

    return oMaterial;
}

void UpdateMaterial(const Material* _pMaterial, const Vector3_s8* _pLightDirection, struct PaletteManager* _pPaletteManager)
{
    const u8 uAlbedoColorsCount = _pMaterial->m_uAlbedoColorsCount;

    //Diffuse constants
    const MaterialDiffuseComponent* pMaterialDiffuseComponent = &_pMaterial->m_oDiffuseComponent;
    const bool bDiffuseIsColorRGB333 = pMaterialDiffuseComponent->m_bIsColorRGB333;
    const u8 uFrontShadeMaxIndex = pMaterialDiffuseComponent->m_uFrontShadeMaxIndex;
    const u8 uFrontSurplusBits = pMaterialDiffuseComponent->m_uFrontSurplusBits;
    const u8 uBackShadeMaxIndex = pMaterialDiffuseComponent->m_uBackShadeMaxIndex;
    const u8 uBackSurplusBits = pMaterialDiffuseComponent->m_uBackSurplusBits;
    const MaterialDiffuseColorShades* pFrontShadedAlbedoColors = pMaterialDiffuseComponent->m_pFrontShadedAlbedoColors;
    const MaterialDiffuseColorShades* pBackShadedAlbedoColors = pMaterialDiffuseComponent->m_pBackShadedAlbedoColors;

    //Specular constants
    const MaterialSpecularComponent* pSpecularComponent = _pMaterial->m_pSpecularComponent;
    const bool bHasSpecular = pSpecularComponent != NULL;
    assert(!bHasSpecular || bDiffuseIsColorRGB333);
    const u8 uSpecularFrontShadeMaxIndex = bHasSpecular ? pSpecularComponent->m_uFrontShadeMaxIndex : 0;
    const u8 uSpecularFrontSurplusBits = bHasSpecular ? pSpecularComponent->m_uFrontSurplusBits : 0;
    const u8 uSpecularBackShadeMaxIndex = bHasSpecular ? pSpecularComponent->m_uBackShadeMaxIndex : 0;
    const u8 uSpecularBackSurplusBits = bHasSpecular ? pSpecularComponent->m_uBackSurplusBits : 0;
    const RGBColor* pSpecularFrontShades = bHasSpecular ? pSpecularComponent->m_pFrontShades : NULL;
    const RGBColor* pSpecularBackShades = bHasSpecular ? pSpecularComponent->m_pBackShades : NULL;
    const Vector3_s8 oHalfVectorFront = (bHasSpecular && uSpecularFrontShadeMaxIndex > 0) ? GetHalfVector(_pLightDirection) : (Vector3_s8){};
    const Vector3_s8 oHalfVectorBack = (bHasSpecular && uSpecularBackShadeMaxIndex > 0) ? GetHalfVector(&(Vector3_s8){-_pLightDirection->m_sX, -_pLightDirection->m_sY, -_pLightDirection->m_sZ}) : (Vector3_s8){};

    const u8 uNormalsCount = _pMaterial->m_uNormalsCount;
    const Vector3_s8* pNormals = _pMaterial->m_pNormals;    
    const NormalsGroup* pCurrentNormalsGroup = _pMaterial->m_pNormalsGroups;
    const NormalsGroup* pNormalsGroupsEnd = &_pMaterial->m_pNormalsGroups[_pMaterial->m_uNormalGroupsCount];
    u8 uCurrentColorIndex = pCurrentNormalsGroup->m_uFirstIndex;
    u8 uCurrentNormalsGroupLastIndex = pCurrentNormalsGroup->m_uLastIndex;
    for(u8 uNormalIndex = 0; uNormalIndex < uNormalsCount; ++uNormalIndex)
    {
        const Vector3_s8* pNormal = &pNormals[uNormalIndex];

        //Diffuse per normal data
        bool bIsDisffuseFrontFacing = 0;
        const u8 uDiffuseLightStrength = GetLightStrength(pNormal, _pLightDirection, &bIsDisffuseFrontFacing);
        const u8 uDiffuseSurplusBitsCount = bIsDisffuseFrontFacing ? uFrontSurplusBits : uBackSurplusBits;
        const u8 uDiffuseShadeMaxIndex = bIsDisffuseFrontFacing ? uFrontShadeMaxIndex : uBackShadeMaxIndex;
        const u8 uDiffuseShadeIndex = GetShadeIndex(uDiffuseLightStrength, uDiffuseSurplusBitsCount, uDiffuseShadeMaxIndex);
        const MaterialDiffuseColorShades* pShadedAlbedoColors = bIsDisffuseFrontFacing ? pFrontShadedAlbedoColors : pBackShadedAlbedoColors;

        //Specular per normal data
        const Vector3_s8* pHalfVector = bIsDisffuseFrontFacing ? &oHalfVectorFront : &oHalfVectorBack;
        const bool bUseSpecular = bHasSpecular && pHalfVector != NULL;
        const u8 uSpecularLightStrength = bUseSpecular ? GetLightStrength(pNormal, pHalfVector, NULL) : 0;
        const u8 uSpecularSurplusBitsCount = bIsDisffuseFrontFacing ? uSpecularFrontSurplusBits : uSpecularBackSurplusBits;
        const u8 uSpecularShadeMaxIndex = bIsDisffuseFrontFacing ? uSpecularFrontShadeMaxIndex : uSpecularBackShadeMaxIndex;
        const u8 uSpecularShadeIndex = bUseSpecular ? GetShadeIndex(uSpecularLightStrength, uSpecularSurplusBitsCount, uSpecularShadeMaxIndex) : 0;
        const RGBColor* pSpecularShades = bIsDisffuseFrontFacing ? pSpecularFrontShades : pSpecularBackShades;
        const RGBColor* pSpecularColor = bUseSpecular ? &pSpecularShades[uSpecularShadeIndex] : NULL;

        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < uAlbedoColorsCount; ++uAlbedoColorIndex)
        {
            const u16 uShadedColor = bUseSpecular ?
                GetDiffuseAndSpecularShadedColor(pShadedAlbedoColors[uAlbedoColorIndex], uDiffuseShadeIndex, pSpecularColor) :
                GetDiffuseShadedColor(pShadedAlbedoColors[uAlbedoColorIndex], uDiffuseShadeIndex);
            SetColorInPaletteManager(_pPaletteManager, uCurrentColorIndex, uShadedColor);
            if(uCurrentColorIndex == uCurrentNormalsGroupLastIndex)
            {
                ++pCurrentNormalsGroup;
                if(pCurrentNormalsGroup != pNormalsGroupsEnd)
                {
                    uCurrentColorIndex = pCurrentNormalsGroup->m_uFirstIndex;
                    uCurrentNormalsGroupLastIndex = pCurrentNormalsGroup->m_uLastIndex;
                }
                else
                    ++uCurrentColorIndex;
            }
            else
                ++uCurrentColorIndex;
        }
    }
    if(_pMaterial->m_bIncludePureBlack)
        SetColorInPaletteManager(_pPaletteManager, uCurrentColorIndex, 0);
}

static u16 GetMaterialExternalDataSize(const Material* _pMaterial)
{
    return sizeof(Vector3_s8) * _pMaterial->m_uNormalsCount +
        GetMaterialDiffuseComponentExternalDataSize(&_pMaterial->m_oDiffuseComponent, _pMaterial->m_uAlbedoColorsCount) +
        GetMaterialSpecularComponentExternalDataSize(_pMaterial->m_pSpecularComponent);
}

//Computes the half vector necessary for the Blinn-Phong specular component
static Vector3_s8 GetHalfVector(const Vector3_s8* _pLightDirection)
{
    //The view vector is always (0, 0, -1) or, in _pLightDirection scale, (0, 0, -128)
    //The half vector is _pLightDirection - view vector
    const s8 sLightX = _pLightDirection->m_sX;
    const s8 sLightY = _pLightDirection->m_sY;
    const s8 sLightZ = _pLightDirection->m_sZ;
    const s16 sHalfVectorZNonNormalized = (s16)sLightZ - 128;
    //The new Z component is in the [0, -256] range
    const f16 fHalfVectorZ = F16_div(FIX16(sHalfVectorZNonNormalized), FIX16(256));

    const f16 fHorizontalMultiplierOld = GetSinFromCos(F16_div(FIX16(sLightZ), FIX16(128)));
    f16 fHorizontalMultiplier = 0;
    if(fHorizontalMultiplierOld != 0)
    {
        const f16 fHorizontalMultiplierNew = GetSinFromCos(fHalfVectorZ);
        //In order to renormalize the XY axis, divide by the old cossinus (to get the normalized 2D vector) and multiply by the new one
        fHorizontalMultiplier = fHorizontalMultiplierOld == 0 ? 0 : F16_div(fHorizontalMultiplierNew, fHorizontalMultiplierOld);
    }
    
    const s16 sHalfVectorX = F16_toInt(F16_mul(FIX16(sLightX), fHorizontalMultiplier));
    const s16 sHalfVectorY = F16_toInt(F16_mul(FIX16(sLightY), fHorizontalMultiplier));
    const s16 sHalfVectorZ = F16_toInt(F16_mul(FIX16(128), fHalfVectorZ));

    return (Vector3_s8){
        .m_sX = clamp(sHalfVectorX, -128, 127),
        .m_sY = clamp(sHalfVectorY, -128, 127),
        .m_sZ = clamp(sHalfVectorZ, -128, 127),
    };
}

//Computes light strength in the [0, 255] range and whether is front or back facing
static u8 GetLightStrength(const Vector3_s8* _pNormal, const Vector3_s8* _pLightDirection, bool* _pIsFrontFacing)
{
    //Vectors are normalized in the [127, -128] range. In order to reamp them to the [1, -1] range, multiply the result by (1/128)^2.
    //Then, remap to the [255, -255] range by multiplying by 255. This is equivalent to multiplying by 255/(128^2) = 1/64.
    s16 sLightStrength = DotProduct(_pNormal, _pLightDirection) / 64;
    sLightStrength = clamp(sLightStrength, -255, 255);
    if(_pIsFrontFacing != NULL)
        *_pIsFrontFacing = sLightStrength > 0;
    return abs(sLightStrength);
}

static u8 GetShadeIndex(u8 _uLightStrength, u8 _uDiffuseSurplusBitsCount, u8 _uDiffuseComponentMax)
{
    //Round towards closest shade index
    u8 uShadeIndex = (_uLightStrength >> _uDiffuseSurplusBitsCount) + ((_uLightStrength & (1 << (_uDiffuseSurplusBitsCount - 1))) == 0 ? 0 : 1);
    return min(uShadeIndex, _uDiffuseComponentMax);
}

static u16 GetDiffuseShadedColor(MaterialDiffuseColorShades _pShades, u8 _uDiffuseShadeIndex)
{
    return _pShades.m_pVDP[_uDiffuseShadeIndex];
}

static u16 GetDiffuseAndSpecularShadedColor(MaterialDiffuseColorShades _pDiffuseShades, u8 _uDiffuseShadeIndex, const RGBColor* _pSpecularColor)
{
    const RGBColor* pDiffuseColor = &_pDiffuseShades.m_pRGB333[_uDiffuseShadeIndex];
    u8 uR = pDiffuseColor->m_uR + _pSpecularColor->m_uR;
    uR = min(uR, 7);
    u8 uG = pDiffuseColor->m_uG + _pSpecularColor->m_uG;
    uG = min(uG, 7);
    u8 uB = pDiffuseColor->m_uB + _pSpecularColor->m_uB;
    uB = min(uB, 7);
    return RGB3_3_3_TO_VDPCOLOR(uR, uG, uB);
}