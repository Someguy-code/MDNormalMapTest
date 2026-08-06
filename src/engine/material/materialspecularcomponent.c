#include "materialspecularcomponent.h"

MaterialSpecularComponent CreateMaterialSpecularComponent(const u8* _pData)
{
    const u8 uFrontLogShadesCount = _pData[0];
    const u8 uBackLogShadesCount = _pData[1];
    const u8 uFrontShadesCount = 1 << uFrontLogShadesCount;
    const u8 uBackShadesCount = 1 << uBackLogShadesCount;

    const RGBColor* pShades = (const RGBColor*)&_pData[2];
    const RGBColor* pFrontShades = pShades;
    const RGBColor* pBackShades = &pShades[uFrontShadesCount];

    return (MaterialSpecularComponent){
        .m_uFrontShadeMaxIndex = uFrontShadesCount - 1,
        .m_uFrontSurplusBits = 8 - uFrontLogShadesCount,
        .m_pFrontShades = pFrontShades,
        .m_uBackShadeMaxIndex = uBackShadesCount - 1,
        .m_uBackSurplusBits = 8 - uBackLogShadesCount,
        .m_pBackShades = pBackShades
    };
}

u16 GetMaterialSpecularComponentExternalDataSize(const MaterialSpecularComponent* _pMaterialSpecularComponent)
{
    if(_pMaterialSpecularComponent == NULL)
        return 0;

    const u8 uFrontShadesCount = _pMaterialSpecularComponent->m_uFrontShadeMaxIndex + 1;
    const u8 uBackShadesCount = _pMaterialSpecularComponent->m_uBackShadeMaxIndex + 1;
    return sizeof(RGBColor) * (uFrontShadesCount + uBackShadesCount);
}

void WrapMutableMaterialSpecularComponentExternalData(const MaterialSpecularComponent* _pOriginalMaterialSpecularComponent, MaterialSpecularComponent** _pTargetMaterialSpecularComponent, u8* _pExternalData)
{
    if(_pOriginalMaterialSpecularComponent == NULL)
    {
        *_pTargetMaterialSpecularComponent = NULL;
        return;
    }

    *_pTargetMaterialSpecularComponent = (MaterialSpecularComponent*)MEM_alloc(sizeof(MaterialSpecularComponent));
    **_pTargetMaterialSpecularComponent = *_pOriginalMaterialSpecularComponent;

    const u8 uFrontShadesSize = sizeof(RGBColor) * (_pOriginalMaterialSpecularComponent->m_uFrontShadeMaxIndex + 1);
    memcpy(_pExternalData, _pOriginalMaterialSpecularComponent->m_pFrontShades, uFrontShadesSize);
    const u8 uBackShadesSize = sizeof(RGBColor) * (_pOriginalMaterialSpecularComponent->m_uBackShadeMaxIndex + 1);
    memcpy(_pExternalData + uFrontShadesSize, _pOriginalMaterialSpecularComponent->m_pBackShades, uBackShadesSize);

    (*_pTargetMaterialSpecularComponent)->m_pFrontShades = (const RGBColor*) _pExternalData;
    (*_pTargetMaterialSpecularComponent)->m_pBackShades = (const RGBColor*) (_pExternalData + uFrontShadesSize);
}