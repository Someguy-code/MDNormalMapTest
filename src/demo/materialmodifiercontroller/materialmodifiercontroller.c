#include "materialmodifiercontroller.h"

#include <engine/constants/colorconstants.h>
#include <engine/input/mouseutils.h>
#include <engine/material/materialspecularcomponent.h>
#include <engine/utils/colorutils.h>

enum
{
    TOGGLE_TEXTURE_FRAMES = 16,
    TOGGLE_DIFFUSE_FRAMES = 16,
    TOGGLE_SPECULAR_FRAMES = 16,
};

static bool UpdateToggleTextureInterpolation(MaterialModifierController* _pMaterialModifierController, u8* _pToggleTextureBalance);
static bool UpdateCycleFeatruesTextureInterpolation(MaterialModifierController* _pMaterialModifierController, bool _bIsTogglingTexture, u8 _uToggleTextureBalance);
static bool UpdateToggleDiffuseInterpolation(MaterialModifierController* _pMaterialModifierController, bool _bIsTogglingTexture, u8 _uToggleTextureBalance);
static bool UpdateToggleSpecularInterpolation(MaterialModifierController* _pMaterialModifierController);
static void ComputeUnlitShadesRGB(MaterialDiffuseColorShades* _pTargetShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uBalance);
static void ComputeUnlitShadesVDP(MaterialDiffuseColorShades* _pTargetShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uBalance);
static void ComputeDiffuseShadesRGB(MaterialDiffuseColorShades* _pTargetShades, const MaterialDiffuseColorShades* _pOriginalShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uToggleDiffuseBalance, u8 _uToggleTextureBalance);
static void ComputeDiffuseShadesVDP(MaterialDiffuseColorShades* _pTargetShades, const MaterialDiffuseColorShades* _pOriginalShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uToggleDiffuseBalance, u8 _uToggleTextureBalance);
static void InterpolateShadesRGB(RGBColor* _pTargetShades, const RGBColor* _pShades1, const RGBColor* _pShades2, u8 _uShadesCount, u8 _uBalance);
static void InterpolateShadesVDP(u16* _pTargetShades, const u16* _pShades1, const u16* _pShades2, u8 _uShadesCount, u8 _uBalance);
static void InterpolateShadesToColorRGB(RGBColor* _pTargetShades, const RGBColor* _pShades, const RGBColor* _pColor, u8 _uShadesCount, u8 _uBalance);
static void ToggleTexture(MaterialModifierController* _pMaterialModifierController);
static void CycleMaterialFeatures(MaterialModifierController* _pMaterialModifierController);
static MaterialFeatures GetNextMaterialFeatures(MaterialFeatures _eCurrentmaterialFeatures, bool _bHasSpecular);
static void ToggleInterpolator(u16Interpolator* _pInterpolator, bool _bCurrentIsSet, u16 _uInterpolationFrames);
static bool IsDiffuseEnabled(MaterialFeatures _eMaterialFeatures);
static bool IsSpecularEnabled(MaterialFeatures _eMaterialFeatures);

u16 InitializeMaterialModifierController(MaterialModifierController* _pMaterialModifierController, const Material* _pOriginalMaterial, struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex)
{
    *_pMaterialModifierController = (MaterialModifierController){
        .m_pOriginalMaterial = _pOriginalMaterial,
        .m_oMutabledMaterialWrapper = CreateMutableMaterilaWrapper(_pOriginalMaterial),
        .m_bUseTexture = true,
        .m_eMaterialFeatures = _pOriginalMaterial->m_pSpecularComponent == NULL ? DIFFUSE_ONLY : DIFFUSE_AND_SPECULAR,
    };

    return InitializeMaterialModifierUI(&_pMaterialModifierController->m_oMaterialModifierUI, _pEngine, _uPaletteIndex, _uBaseTileIndex, _pOriginalMaterial->m_pSpecularComponent != NULL);
}

void DestroyMaterialModifierController(MaterialModifierController* _pMaterialModifierController)
{
    DestroyMutableMaterialWrapper(&_pMaterialModifierController->m_oMutabledMaterialWrapper);
}

bool UpdateMaterialModifierController(MaterialModifierController* _pMaterialModifierController)
{
    const bool bUseTexture = _pMaterialModifierController->m_bUseTexture;
    const MaterialFeatures eMaterialFeatures = _pMaterialModifierController->m_eMaterialFeatures;
    UpdateMaterialModifierUI(&_pMaterialModifierController->m_oMaterialModifierUI, bUseTexture, IsDiffuseEnabled(eMaterialFeatures), IsSpecularEnabled(eMaterialFeatures));

    u8 uToggleTextureBalance = bUseTexture ? MAX_COLOR_COMPONENT : 0;
    const bool bIsTogglingTexture = UpdateToggleTextureInterpolation(_pMaterialModifierController, &uToggleTextureBalance);
    const bool bIsCyclingFeatures = UpdateCycleFeatruesTextureInterpolation(_pMaterialModifierController, bIsTogglingTexture, uToggleTextureBalance);
    return bIsTogglingTexture || bIsCyclingFeatures;
}

void JoyHandlerMaterialModifierController(MaterialModifierController* _pMaterialModifierController, u16 _uJoyID, u16 _uChanged, u16 _uState)
{
    bool bToggleTexture = false;
    bool bCycleMaterialFeatures = false;
    switch(_uJoyID)
    {
        case JOY_1:
            bToggleTexture = _uState & BUTTON_A;
            bCycleMaterialFeatures = _uState & BUTTON_B;
            break;
        case JOY_2:
            bToggleTexture = CheckMouseButtons(_uJoyID, _uChanged, _uState, BUTTON_LMB);
            bCycleMaterialFeatures = CheckMouseButtons(_uJoyID, _uChanged, _uState, BUTTON_RMB);
            break;
        default:
            break;
    };

    if(bToggleTexture)
        ToggleTexture(_pMaterialModifierController);

    if(bCycleMaterialFeatures)
        CycleMaterialFeatures(_pMaterialModifierController);
}

static bool UpdateToggleTextureInterpolation(MaterialModifierController* _pMaterialModifierController, u8* _pToggleTextureBalance)
{
    u16Interpolator* pTextureInterpolator = &_pMaterialModifierController->m_oTextureInterpolator;
    const bool bIsTogglingTexture = !IsU16InterpolatorFinished(pTextureInterpolator);
    if(bIsTogglingTexture)
    {
        const RGBColor* pAlbedoColors = _pMaterialModifierController->m_pOriginalMaterial->m_pAlbedoColors;
        Material* pTargetMaterial = &_pMaterialModifierController->m_oMutabledMaterialWrapper.m_oMaterial;
        MaterialDiffuseComponent* pTargetMaterialDiffuseComponent = &pTargetMaterial->m_oDiffuseComponent;
        MaterialDiffuseColorShades* pTargetFrontShades = pTargetMaterialDiffuseComponent->m_pFrontShadedAlbedoColors;
        MaterialDiffuseColorShades* pTargetBackShades = pTargetMaterialDiffuseComponent->m_pBackShadedAlbedoColors;
        const u8 uFrontShadesCount = pTargetMaterialDiffuseComponent->m_uFrontShadeMaxIndex + 1;
        const u8 uBackShadesCount = pTargetMaterialDiffuseComponent->m_uFrontShadeMaxIndex + 1;
        const u8 uAlbedoColorsCount = pTargetMaterial->m_uAlbedoColorsCount;
        const bool bIsColorRGB333 = pTargetMaterialDiffuseComponent->m_bIsColorRGB333;
        const u8 uBalance = IsU16InterpolatorInitialized(pTextureInterpolator) ? UpdateU16Interpolator(pTextureInterpolator) : MAX_COLOR_COMPONENT;
        *_pToggleTextureBalance = uBalance;
        const bool bIsTogglingDiffuse = !IsU16InterpolatorFinished(&_pMaterialModifierController->m_oDiffuseInterpolator);
        if(!bIsTogglingDiffuse)
        {
            if(bIsColorRGB333)
            {
                ComputeUnlitShadesRGB(pTargetFrontShades, uFrontShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance);
                ComputeUnlitShadesRGB(pTargetBackShades, uBackShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance);
            }
            else
            {
                ComputeUnlitShadesVDP(pTargetFrontShades, uFrontShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance);
                ComputeUnlitShadesVDP(pTargetBackShades, uBackShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance);
            }
        }
    }
    return bIsTogglingTexture;
}

static bool UpdateCycleFeatruesTextureInterpolation(MaterialModifierController* _pMaterialModifierController, bool _bIsTogglingTexture, u8 _uToggleTextureBalance)
{
    const bool bIsTogglingDiffuse = UpdateToggleDiffuseInterpolation(_pMaterialModifierController, _bIsTogglingTexture, _uToggleTextureBalance);
    const bool bIsTogglingSpecular = UpdateToggleSpecularInterpolation(_pMaterialModifierController);
    return bIsTogglingDiffuse || bIsTogglingSpecular;
}

static bool UpdateToggleDiffuseInterpolation(MaterialModifierController* _pMaterialModifierController, bool _bIsTogglingTexture, u8 _uToggleTextureBalance)
{
    u16Interpolator* pDiffuseInterpolator = &_pMaterialModifierController->m_oDiffuseInterpolator;
    const bool bIsTogglingDiffuse = !IsU16InterpolatorFinished(pDiffuseInterpolator);
    if(bIsTogglingDiffuse || (_bIsTogglingTexture && IsDiffuseEnabled(_pMaterialModifierController->m_eMaterialFeatures)))
    {
        const RGBColor* pAlbedoColors = _pMaterialModifierController->m_pOriginalMaterial->m_pAlbedoColors;
        const MaterialDiffuseComponent* pOriginalMaterialDiffuseComponent = &_pMaterialModifierController->m_pOriginalMaterial->m_oDiffuseComponent;
        const MaterialDiffuseColorShades* pOriginalFrontShades = pOriginalMaterialDiffuseComponent->m_pFrontShadedAlbedoColors;
        const MaterialDiffuseColorShades* pOriginalBackShades = pOriginalMaterialDiffuseComponent->m_pBackShadedAlbedoColors;
        Material* pTargetMaterial = &_pMaterialModifierController->m_oMutabledMaterialWrapper.m_oMaterial;
        MaterialDiffuseComponent* pTargetMaterialDiffuseComponent = &pTargetMaterial->m_oDiffuseComponent;
        MaterialDiffuseColorShades* pTargetFrontShades = pTargetMaterialDiffuseComponent->m_pFrontShadedAlbedoColors;
        MaterialDiffuseColorShades* pTargetBackShades = pTargetMaterialDiffuseComponent->m_pBackShadedAlbedoColors;
        const u8 uFrontShadesCount = pTargetMaterialDiffuseComponent->m_uFrontShadeMaxIndex + 1;
        const u8 uBackShadesCount = pTargetMaterialDiffuseComponent->m_uFrontShadeMaxIndex + 1;
        const u8 uAlbedoColorsCount = pTargetMaterial->m_uAlbedoColorsCount;
        const bool bIsColorRGB333 = pTargetMaterialDiffuseComponent->m_bIsColorRGB333;
        const u8 uBalance = IsU16InterpolatorInitialized(pDiffuseInterpolator) ? UpdateU16Interpolator(pDiffuseInterpolator) : MAX_COLOR_COMPONENT;
        if(bIsColorRGB333)
        {
            ComputeDiffuseShadesRGB(pTargetFrontShades, pOriginalFrontShades, uFrontShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance, _uToggleTextureBalance);
            ComputeDiffuseShadesRGB(pTargetBackShades, pOriginalBackShades, uBackShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance, _uToggleTextureBalance);
        }
        else
        {
            ComputeDiffuseShadesVDP(pTargetFrontShades, pOriginalFrontShades, uFrontShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance, _uToggleTextureBalance);
            ComputeDiffuseShadesVDP(pTargetBackShades, pOriginalBackShades, uBackShadesCount, pAlbedoColors, uAlbedoColorsCount, uBalance, _uToggleTextureBalance);
        }
    }
    return bIsTogglingDiffuse;
}

static bool UpdateToggleSpecularInterpolation(MaterialModifierController* _pMaterialModifierController)
{
    u16Interpolator* pSpecularInterpolator = &_pMaterialModifierController->m_oSpecularInterpolator;
    const bool bIsTogglingSpecular = !IsU16InterpolatorFinished(pSpecularInterpolator);
    if(bIsTogglingSpecular)
    {
        const MaterialSpecularComponent* pOriginalMaterialSpecularComponent = _pMaterialModifierController->m_pOriginalMaterial->m_pSpecularComponent;
        const RGBColor* pOriginalFrontShades = pOriginalMaterialSpecularComponent->m_pFrontShades;
        const RGBColor* pOriginalBackShades = pOriginalMaterialSpecularComponent->m_pBackShades;
        Material* pTargetMaterial = &_pMaterialModifierController->m_oMutabledMaterialWrapper.m_oMaterial;
        MaterialSpecularComponent* pTargetMaterialSpecularComponent = pTargetMaterial->m_pSpecularComponent;
        RGBColor* pTargetFrontShades = (RGBColor*)pTargetMaterialSpecularComponent->m_pFrontShades;
        RGBColor* pTargetBackShades = (RGBColor*)pTargetMaterialSpecularComponent->m_pBackShades;
        const u8 uFrontShadesCount = pTargetMaterialSpecularComponent->m_uFrontShadeMaxIndex + 1;
        const u8 uBackShadesCount = pTargetMaterialSpecularComponent->m_uFrontShadeMaxIndex + 1;
        const u8 uBalance = IsU16InterpolatorInitialized(pSpecularInterpolator) ? UpdateU16Interpolator(pSpecularInterpolator) : MAX_COLOR_COMPONENT;
        static const RGBColor oBlack = {0,0,0};
        InterpolateShadesToColorRGB(pTargetFrontShades, pOriginalFrontShades, &oBlack, uFrontShadesCount, uBalance);
        InterpolateShadesToColorRGB(pTargetBackShades, pOriginalBackShades, &oBlack, uBackShadesCount, uBalance);
    }
    return bIsTogglingSpecular;
}

static void ComputeUnlitShadesRGB(MaterialDiffuseColorShades* _pTargetShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uBalance)
{
    const RGBColor* pFirstColor = _pAlbedoColors;
    for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
    {
        RGBColor* pTargetShades =  (RGBColor*)_pTargetShades[uAlbedoColorIndex].m_pRGB333;
        const RGBColor* pAlbedoColor = &_pAlbedoColors[uAlbedoColorIndex];
        const RGBColor oInterpolatedAlbedoColor = uAlbedoColorIndex == 0 ? *pAlbedoColor :
            (RGBColor)RGBCOLOR333_LERP(*pFirstColor, *pAlbedoColor, _uBalance);
        for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
            pTargetShades[uShadeIndex] = oInterpolatedAlbedoColor;
    }
}

static void ComputeUnlitShadesVDP(MaterialDiffuseColorShades* _pTargetShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uBalance)
{
    const RGBColor* pFirstColor = _pAlbedoColors;
    for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
    {
        u16* pTargetShades =  (u16*)_pTargetShades[uAlbedoColorIndex].m_pVDP;
        const RGBColor* pAlbedoColor = &_pAlbedoColors[uAlbedoColorIndex];
        const RGBColor oInterpolatedAlbedoColor = uAlbedoColorIndex == 0 ? *pAlbedoColor :
            (RGBColor)RGBCOLOR333_LERP(*pFirstColor, *pAlbedoColor, _uBalance);
        const u16 uInterpolatedAlbedoColorVDP = RGBCOLOR_333_TO_VDPCOLOR(oInterpolatedAlbedoColor);
        for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
            pTargetShades[uShadeIndex] = uInterpolatedAlbedoColorVDP;
    }
}

static void ComputeDiffuseShadesRGB(MaterialDiffuseColorShades* _pTargetShades, const MaterialDiffuseColorShades* _pOriginalShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uToggleDiffuseBalance, u8 _uToggleTextureBalance)
{
    const RGBColor* pFirstOriginalShades = _pOriginalShades->m_pRGB333;
    if(_uToggleDiffuseBalance == MAX_COLOR_COMPONENT)
    {
        //The texture is fully visible, so copy the the original diffuse shades verbatim
        if(_uToggleTextureBalance == MAX_COLOR_COMPONENT)
            memcpy((void*)_pTargetShades->m_pRGB333, (const void*)pFirstOriginalShades, sizeof(RGBColor) * _uShadesCount * _uAlbedoColorsCount);
        else
        {
            memcpy((void*)_pTargetShades->m_pRGB333, (const void*)pFirstOriginalShades, sizeof(RGBColor) * _uShadesCount);
            if(_uToggleTextureBalance == 0)
            {
                ///The texture is fully invisible, so copy the original shades of the first albedo to all the rest
                for(u8 uAlbedoColorIndex = 1; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
                    memcpy((void*)_pTargetShades[uAlbedoColorIndex].m_pRGB333, (const void*)pFirstOriginalShades, sizeof(RGBColor) * _uShadesCount);
            }
            else
            {
                //Just interpolate between the first albedo and the rest
                for(u8 uAlbedoColorIndex = 1; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
                    InterpolateShadesRGB((RGBColor*)_pTargetShades[uAlbedoColorIndex].m_pRGB333, pFirstOriginalShades, _pOriginalShades[uAlbedoColorIndex].m_pRGB333, _uShadesCount, _uToggleTextureBalance);
            }
        }
    }
    else
    {
        const RGBColor* pFirstAlbedoColor = _pAlbedoColors;
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
        {
            const RGBColor* pAlbedoColor = &_pAlbedoColors[uAlbedoColorIndex];
            RGBColor* pTargetAlbedoShades = (RGBColor*)_pTargetShades[uAlbedoColorIndex].m_pRGB333;
            const RGBColor* pOriginalAlbedoShades = _pOriginalShades[uAlbedoColorIndex].m_pRGB333;
            for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
            {
                const RGBColor* pShade1 = &pFirstOriginalShades[uShadeIndex];
                const RGBColor* pShade2 = &pOriginalAlbedoShades[uShadeIndex];
                const RGBColor oLitShade = RGBCOLOR333_LERP(*pShade1, *pShade2, _uToggleTextureBalance);
                const RGBColor oUnlitShade =
                    _uToggleTextureBalance == 0 ? *pFirstAlbedoColor : 
                    _uToggleTextureBalance == MAX_COLOR_COMPONENT ? *pAlbedoColor :
                    (RGBColor)RGBCOLOR333_LERP(*pFirstAlbedoColor, *pAlbedoColor, _uToggleTextureBalance);
                pTargetAlbedoShades[uShadeIndex] = (RGBColor)RGBCOLOR333_LERP(oUnlitShade, oLitShade, _uToggleDiffuseBalance);
            }
        }
    }
}

static void ComputeDiffuseShadesVDP(MaterialDiffuseColorShades* _pTargetShades, const MaterialDiffuseColorShades* _pOriginalShades, u8 _uShadesCount, const RGBColor* _pAlbedoColors, u8 _uAlbedoColorsCount, u8 _uToggleDiffuseBalance, u8 _uToggleTextureBalance)
{
    const u16* pFirstOriginalShades = _pOriginalShades->m_pVDP;
    if(_uToggleDiffuseBalance == MAX_COLOR_COMPONENT)
    {
        //The texture is fully visible, so copy the the original diffuse shades verbatim
        if(_uToggleTextureBalance == MAX_COLOR_COMPONENT)
            memcpy((void*)_pTargetShades->m_pVDP, (const void*)pFirstOriginalShades, sizeof(u16) * _uShadesCount * _uAlbedoColorsCount);
        else
        {
            //The first albedo is always the original
            memcpy((void*)_pTargetShades->m_pVDP, (const void*)pFirstOriginalShades, sizeof(u16) * _uShadesCount);
            if(_uToggleTextureBalance == 0)
            {
                ///The texture is fully invisible, so copy the original shades of the first albedo to all the rest
                for(u8 uAlbedoColorIndex = 1; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
                    memcpy((void*)_pTargetShades[uAlbedoColorIndex].m_pVDP, (const void*)pFirstOriginalShades, sizeof(u16) * _uShadesCount);
            }
            else
            {
                //Just interpolate between the first albedo and the rest
                for(u8 uAlbedoColorIndex = 1; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
                    InterpolateShadesVDP((u16*)_pTargetShades[uAlbedoColorIndex].m_pVDP, pFirstOriginalShades, _pOriginalShades[uAlbedoColorIndex].m_pVDP, _uShadesCount, _uToggleTextureBalance);
            }
        }
    }
    else
    {
        const u16 uFirstAlbedoColor = RGBCOLOR_333_TO_VDPCOLOR(*_pAlbedoColors);
        for(u8 uAlbedoColorIndex = 0; uAlbedoColorIndex <_uAlbedoColorsCount; ++uAlbedoColorIndex)
        {
            const RGBColor* pAlbedoColor = &_pAlbedoColors[uAlbedoColorIndex];
            const u16 uAlbedoColor = RGBCOLOR_333_TO_VDPCOLOR(*pAlbedoColor);
            u16* pTargetAlbedoShades = (u16*)_pTargetShades[uAlbedoColorIndex].m_pVDP;
            const u16* pOriginalAlbedoShades = _pOriginalShades[uAlbedoColorIndex].m_pVDP;
            for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
            {
                const u16 uShade1 = pFirstOriginalShades[uShadeIndex];
                const u16 uShade2 = pOriginalAlbedoShades[uShadeIndex];
                const u16 uLitShade = VDPCOLOR_LERP(uShade1, uShade2, _uToggleTextureBalance);
                const u16 uUnlitShade =
                    _uToggleTextureBalance == 0 ? uFirstAlbedoColor : 
                    _uToggleTextureBalance == MAX_COLOR_COMPONENT ? uAlbedoColor :
                    VDPCOLOR_LERP(uFirstAlbedoColor, uAlbedoColor, _uToggleTextureBalance);
                pTargetAlbedoShades[uShadeIndex] = VDPCOLOR_LERP(uUnlitShade, uLitShade, _uToggleDiffuseBalance);
            }
        }
    }
}

static void InterpolateShadesRGB(RGBColor* _pTargetShades, const RGBColor* _pShades1, const RGBColor* _pShades2, u8 _uShadesCount, u8 _uBalance)
{
    for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
    {
        const RGBColor* pShade1 = &_pShades1[uShadeIndex];
        const RGBColor* pShade2 = &_pShades2[uShadeIndex];
        _pTargetShades[uShadeIndex] = (RGBColor)RGBCOLOR333_LERP(*pShade1, *pShade2, _uBalance);
    }
}

static void InterpolateShadesVDP(u16* _pTargetShades, const u16* _pShades1, const u16* _pShades2, u8 _uShadesCount, u8 _uBalance)
{
    for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
    {
        const u16 uShade1 = _pShades1[uShadeIndex];
        const u16 uShade2 = _pShades2[uShadeIndex];
        _pTargetShades[uShadeIndex] = VDPCOLOR_LERP(uShade1, uShade2, _uBalance);
    }
}

static void InterpolateShadesToColorRGB(RGBColor* _pTargetShades, const RGBColor* _pShades, const RGBColor* _pColor, u8 _uShadesCount, u8 _uBalance)
{
    for(u8 uShadeIndex = 0; uShadeIndex < _uShadesCount; ++uShadeIndex)
    {
        const RGBColor* pShade = &_pShades[uShadeIndex];
        _pTargetShades[uShadeIndex] = (RGBColor)RGBCOLOR333_LERP(*_pColor, *pShade, _uBalance);
    }
}

static void ToggleTexture(MaterialModifierController* _pMaterialModifierController)
{
    const bool bCurrentUseTexture = _pMaterialModifierController->m_bUseTexture;
    ToggleInterpolator(&_pMaterialModifierController->m_oTextureInterpolator, bCurrentUseTexture, TOGGLE_TEXTURE_FRAMES);
    _pMaterialModifierController->m_bUseTexture = !bCurrentUseTexture;
}

static void CycleMaterialFeatures(MaterialModifierController* _pMaterialModifierController)
{
    const MaterialFeatures eCurrentMaterialFeatures = _pMaterialModifierController->m_eMaterialFeatures;
    const MaterialFeatures eNextMaterialFeatures = GetNextMaterialFeatures(eCurrentMaterialFeatures, _pMaterialModifierController->m_pOriginalMaterial->m_pSpecularComponent != NULL);
    const bool bCurrentUseDiffuse = IsDiffuseEnabled(eCurrentMaterialFeatures);
    const bool bCurrentUseSpecular = IsSpecularEnabled(eCurrentMaterialFeatures);
    if(bCurrentUseDiffuse != IsDiffuseEnabled(eNextMaterialFeatures))
        ToggleInterpolator(&_pMaterialModifierController->m_oDiffuseInterpolator, bCurrentUseDiffuse, TOGGLE_DIFFUSE_FRAMES);
    if(bCurrentUseSpecular != IsSpecularEnabled(eNextMaterialFeatures))
        ToggleInterpolator(&_pMaterialModifierController->m_oSpecularInterpolator, bCurrentUseSpecular, TOGGLE_SPECULAR_FRAMES);
    
    _pMaterialModifierController->m_eMaterialFeatures = eNextMaterialFeatures;   
}

[[gnu::pure]] static MaterialFeatures GetNextMaterialFeatures(MaterialFeatures _eCurrentmaterialFeatures, bool _bHasSpecular)
{
    MaterialFeatures eNextMaterialFeatures = _eCurrentmaterialFeatures + 1;
    if(!_bHasSpecular)
    {
        while(IsSpecularEnabled(eNextMaterialFeatures))
        {
            eNextMaterialFeatures = GetNextMaterialFeatures(eNextMaterialFeatures, _bHasSpecular);
        }
    }
    return eNextMaterialFeatures == MATERIAL_FEATURES_COUNT ? NO_LIGHTING : eNextMaterialFeatures;
}

static void ToggleInterpolator(u16Interpolator* _pInterpolator, bool _bCurrentIsSet, u16 _uInterpolationFrames)
{
    if(IsU16InterpolatorFinished(_pInterpolator))
        InitializeU16Interpolator(_pInterpolator, _bCurrentIsSet ? MAX_COLOR_COMPONENT : 0, _bCurrentIsSet ? 0 : MAX_COLOR_COMPONENT, _uInterpolationFrames);
    else
        FlipU16Interpolator(_pInterpolator, _bCurrentIsSet ? 0 : MAX_COLOR_COMPONENT, _uInterpolationFrames);
}

[[gnu::pure]] static bool IsDiffuseEnabled(MaterialFeatures _eMaterialFeatures)
{
    return (_eMaterialFeatures & DIFFUSE_ONLY) != 0;
}

[[gnu::pure]] static bool IsSpecularEnabled(MaterialFeatures _eMaterialFeatures)
{
    return (_eMaterialFeatures & SPECULAR_ONLY) != 0;
}