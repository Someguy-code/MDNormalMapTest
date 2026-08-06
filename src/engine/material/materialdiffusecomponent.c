#include "materialdiffusecomponent.h"

static u16 GetColorShadesOffset(const u8* _pData, bool _bIsColorRGB333);

MaterialDiffuseComponent CreateMaterialDiffuseComponent(const u8* _pData, u8 _uAlbedoColorsCount, u16* _pMaterialDiffuseDataSize)
{
    const bool bIsColorRGB333 = _pData[0];
    const u8 uFrontLogShadesCount = _pData[1];
    const u8 uBackLogShadesCount = _pData[2];
    const u8 uFrontShadesCount = 1 << uFrontLogShadesCount;
    const u8 uBackShadesCount = 1 << uBackLogShadesCount;

    //Allocate a single contignuous pointers array for both front and back shades.
    MaterialDiffuseColorShades* pShadedAlbedoColors = (MaterialDiffuseColorShades*)MEM_alloc(2 * _uAlbedoColorsCount * sizeof(MaterialDiffuseColorShades));
        
    MaterialDiffuseColorShades* pShadedAlbedoColorsFront = pShadedAlbedoColors;
    MaterialDiffuseColorShades* pShadedAlbedoColorsBack = &pShadedAlbedoColors[_uAlbedoColorsCount];

    const u16 uColorShadesOffset = GetColorShadesOffset(_pData, bIsColorRGB333);
    if(bIsColorRGB333)
    {
        const RGBColor* pNextAlbedoColorShade = (const RGBColor*)&_pData[uColorShadesOffset];
        
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uFrontShadesCount)
            pShadedAlbedoColorsFront[uAlbedoColorIndex].m_pRGB333 = pNextAlbedoColorShade;
        
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uBackShadesCount)
            pShadedAlbedoColorsBack[uAlbedoColorIndex].m_pRGB333 = pNextAlbedoColorShade;
    }
    else
    {
        const u16* pNextAlbedoColorShade = (const u16*)&_pData[uColorShadesOffset];
        
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uFrontShadesCount)
            pShadedAlbedoColorsFront[uAlbedoColorIndex].m_pVDP = pNextAlbedoColorShade;

        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uBackShadesCount)
            pShadedAlbedoColorsBack[uAlbedoColorIndex].m_pVDP = pNextAlbedoColorShade;
    }

    *_pMaterialDiffuseDataSize = uColorShadesOffset + (uFrontShadesCount  + uBackShadesCount) * _uAlbedoColorsCount * 
        (bIsColorRGB333 ? sizeof(RGBColor) : sizeof(u16));

    return (MaterialDiffuseComponent){
        .m_bIsColorRGB333 = bIsColorRGB333,
        .m_uFrontShadeMaxIndex = uFrontShadesCount - 1,
        .m_uFrontSurplusBits = 8 - uFrontLogShadesCount,
        .m_pFrontShadedAlbedoColors = pShadedAlbedoColorsFront,
        .m_uBackShadeMaxIndex = uBackShadesCount - 1,
        .m_uBackSurplusBits = 8 - uBackLogShadesCount,
        .m_pBackShadedAlbedoColors = pShadedAlbedoColorsBack
    };
}

void DestroyMaterialDiffuseComponent(MaterialDiffuseComponent* _pMaterialDiffuseComponent)
{
    //This will destroy both front and back shades
    MEM_free(_pMaterialDiffuseComponent->m_pFrontShadedAlbedoColors);
}

u16 GetMaterialDiffuseComponentExternalDataSize(const MaterialDiffuseComponent* _pMaterialDiffuseComponent, u8 _uAlbedoColorsCount)
{
    const u8 uFrontShadesCount = _pMaterialDiffuseComponent->m_uFrontShadeMaxIndex + 1;
    const u8 uBackShadesCount = _pMaterialDiffuseComponent->m_uBackShadeMaxIndex + 1;
    const bool bIsColorRGB333 = _pMaterialDiffuseComponent->m_bIsColorRGB333;
    const u8 uShadeSize = bIsColorRGB333 ? sizeof(RGBColor) : sizeof(u16);
    //Depending on the alignament of the object, VDP colors may need 1 byte padding so they are 16-bit aligned
    const u8 uPotentialPadding = bIsColorRGB333 ? 0 : 1;
    return _uAlbedoColorsCount * uShadeSize * (uFrontShadesCount + uBackShadesCount) + uPotentialPadding;
}

u8* WrapMutableMaterialDiffuseComponentExternalData(const MaterialDiffuseComponent* _pOriginalMaterialDiffuseComponent, MaterialDiffuseComponent* _pTargetMaterialDiffuseComponent, u8 _uAlbedoColorsCount, u8* _pExternalData)
{
    *_pTargetMaterialDiffuseComponent = *_pOriginalMaterialDiffuseComponent;
    const bool bIsColorRGB333 = _pOriginalMaterialDiffuseComponent->m_bIsColorRGB333;
    const u8 uShadeSize = bIsColorRGB333 ? sizeof(RGBColor) : sizeof(u16);
    const u8 uFrontShadesCount = _pOriginalMaterialDiffuseComponent->m_uFrontShadeMaxIndex + 1;
    const u8 uBackShadesCount = _pOriginalMaterialDiffuseComponent->m_uBackShadeMaxIndex + 1;
    const u8 uFrontShadesSize = _uAlbedoColorsCount * uShadeSize * uFrontShadesCount;
    const u8 uBackShadesSize = _uAlbedoColorsCount * uShadeSize * uBackShadesCount;

    MaterialDiffuseColorShades* pShadedAlbedoColors = (MaterialDiffuseColorShades*)MEM_alloc(2 * _uAlbedoColorsCount * sizeof(MaterialDiffuseColorShades));
    MaterialDiffuseColorShades* pShadedAlbedoColorsFront = pShadedAlbedoColors;
    MaterialDiffuseColorShades* pShadedAlbedoColorsBack = &pShadedAlbedoColors[_uAlbedoColorsCount];
    _pTargetMaterialDiffuseComponent->m_pFrontShadedAlbedoColors = pShadedAlbedoColorsFront;
    _pTargetMaterialDiffuseComponent->m_pBackShadedAlbedoColors = pShadedAlbedoColorsBack;

    const MaterialDiffuseColorShades* pOriginalFrontShades = _pOriginalMaterialDiffuseComponent->m_pFrontShadedAlbedoColors;
    const MaterialDiffuseColorShades* pOriginalBackShades = _pOriginalMaterialDiffuseComponent->m_pBackShadedAlbedoColors;

    u8 uPadding = 0;
    if(bIsColorRGB333)
    {
        const RGBColor* pNextAlbedoColorShade = (RGBColor*)_pExternalData;
        
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uFrontShadesCount)
            pShadedAlbedoColorsFront[uAlbedoColorIndex].m_pRGB333 = pNextAlbedoColorShade;
        
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uBackShadesCount)
            pShadedAlbedoColorsBack[uAlbedoColorIndex].m_pRGB333 = pNextAlbedoColorShade;

        memcpy(_pExternalData, pOriginalFrontShades->m_pRGB333, uFrontShadesSize);
        memcpy(_pExternalData + uFrontShadesSize, pOriginalBackShades->m_pRGB333, uBackShadesSize);
    }
    else
    {
        //Make sure VDP colors are 16-bit aligned
        uPadding = (u32)_pExternalData & 1;
        u8* pPaddedExternalData = _pExternalData + uPadding;
        const u16* pNextAlbedoColorShade = (const u16*)pPaddedExternalData;
        
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uFrontShadesCount)
            pShadedAlbedoColorsFront[uAlbedoColorIndex].m_pVDP = pNextAlbedoColorShade;

        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex < _uAlbedoColorsCount; ++uAlbedoColorIndex, pNextAlbedoColorShade += uBackShadesCount)
            pShadedAlbedoColorsBack[uAlbedoColorIndex].m_pVDP = pNextAlbedoColorShade;

        memcpy(pPaddedExternalData, pOriginalFrontShades->m_pVDP, uFrontShadesSize);
        memcpy(pPaddedExternalData + uFrontShadesSize, pOriginalBackShades->m_pVDP, uBackShadesSize);
    }

    return _pExternalData + uPadding + uFrontShadesSize + uBackShadesSize;
}

[[gnu::pure]] static u16 GetColorShadesOffset(const u8* _pData, bool _bIsColorRGB333)
{
    const u16 uBaseShadesColorOffset = 3;
    //If the address is odd and the color is 16bit wide, there is one byte of padding
    return uBaseShadesColorOffset + (_bIsColorRGB333 ? 0 : ((u32)&_pData[uBaseShadesColorOffset] & 0b1));
}