#ifndef _COLOR_CONSTANTS_H_
#define _COLOR_CONSTANTS_H_

#include <genesis.h>

enum ColorConstants
{
    COMPONENT_VALUE_COUNT = 8,
    MAX_COLOR_COMPONENT = COMPONENT_VALUE_COUNT - 1
};

enum VDPColorConstants
{
    VDP_COLOR_BLACK = RGB3_3_3_TO_VDPCOLOR(0, 0, 0),
    VDP_COLOR_WHITE = RGB3_3_3_TO_VDPCOLOR(7, 7, 7)
};

#endif //#ifndef _COLOR_CONSTANTS_H_