#ifndef _MUTABLE_MATERIAL_WRAPPER_H_
#define _MUTABLE_MATERIAL_WRAPPER_H_

#include <engine/material/material.h>

#include <genesis.h>

//Materials are immutable so they can get their data directly from the ROM. This wrapper creates a copy in RAM in case we need to manipulate it
typedef struct MutableMaterialWrapper
{
    Material m_oMaterial;
    u8* m_pMutableData;
} MutableMaterialWrapper;

MutableMaterialWrapper CreateMutableMaterilaWrapper(const Material* _pOriginalMaterial);
void DestroyMutableMaterialWrapper(MutableMaterialWrapper* _pMutableMaterialWrapper);

#endif //#ifndef _MUTABLE_MATERIAL_WRAPPER_H_