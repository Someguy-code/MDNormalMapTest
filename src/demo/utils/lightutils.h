#ifndef _LIGHT_UTILS_H_
#define _LIGHT_UTILS_H_

#include <genesis.h>

#include <engine/types/vector3.h>

Vect3D_f16 GetLightDirection(s16 _sDeltaCenterX, s16 _sDeltaCenterY, u16 _uMaxDistance);
Vector3_s8 GetLightDirectionS8(const Vect3D_f16* _pLightDirection);

#endif //#ifndef _LIGHT_UTILS_H_