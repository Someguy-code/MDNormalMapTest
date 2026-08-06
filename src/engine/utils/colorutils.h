#ifndef _COLOR_UTILS_H_
#define _COLOR_UTILS_H_

#include <engine/constants/colorconstants.h>

#include <genesis.h>

extern const u8 m_oColorComponentLerpLUT[COMPONENT_VALUE_COUNT][COMPONENT_VALUE_COUNT][COMPONENT_VALUE_COUNT];

#define GET_VDPCOLOR_R(uVDPColor) (((uVDPColor) & VDPPALETTE_REDMASK ) >> VDPPALETTE_REDSFT)
#define GET_VDPCOLOR_G(uVDPColor) (((uVDPColor) & VDPPALETTE_GREENMASK ) >> VDPPALETTE_GREENSFT)
#define GET_VDPCOLOR_B(uVDPColor) (((uVDPColor) & VDPPALETTE_BLUEMASK ) >> VDPPALETTE_BLUESFT)

#define RGBCOLOR_333_TO_VDPCOLOR(_oRGBColor) (RGB3_3_3_TO_VDPCOLOR((_oRGBColor).m_uR, (_oRGBColor).m_uG, (_oRGBColor).m_uB))
#define VDPCOLOR_TO_RGBCOLOR_333(_uVDPColor) {GET_VDPCOLOR_R(_uVDPColor), GET_VDPCOLOR_G(_uVDPColor), GET_VDPCOLOR_B(_uVDPColor)}

#define RGBCOLOR333_LERP(_oColor1, _oColor2, _uBalance) { \
    .m_uR = m_oColorComponentLerpLUT[(_oColor1).m_uR][(_oColor2).m_uR][_uBalance], \
    .m_uG = m_oColorComponentLerpLUT[(_oColor1).m_uG][(_oColor2).m_uG][_uBalance], \
    .m_uB = m_oColorComponentLerpLUT[(_oColor1).m_uB][(_oColor2).m_uB][_uBalance]}

#define RGB333_LERP(_uR1, _uG1, _uB1, _uR2, _uG2, _uB2, _uBalance) (RGB3_3_3_TO_VDPCOLOR( \
    m_oColorComponentLerpLUT[_uR1][_uR2][_uBalance], \
    m_oColorComponentLerpLUT[_uG1][_uG2][_uBalance], \
    m_oColorComponentLerpLUT[_uB1][_uB2][_uBalance]))

#define VDPCOLOR_LERP(_uVDPColor1, _uVDPColor2, _uBalance) (RGB333_LERP( \
    GET_VDPCOLOR_R(_uVDPColor1), GET_VDPCOLOR_G(_uVDPColor1), GET_VDPCOLOR_B(_uVDPColor1), \
    GET_VDPCOLOR_R(_uVDPColor2), GET_VDPCOLOR_G(_uVDPColor2), GET_VDPCOLOR_B(_uVDPColor2), \
    _uBalance))

#endif //#ifndef _COLOR_UTILS_H_