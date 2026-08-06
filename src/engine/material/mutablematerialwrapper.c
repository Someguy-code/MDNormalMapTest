#include "mutablematerialwrapper.h"

#include <engine/material/material.h>

MutableMaterialWrapper CreateMutableMaterilaWrapper(const Material* _pOriginalMaterial)
{
    u8* pMutableData = NULL;
    Material oMaterial = WrapMutableMaterialExternalData(_pOriginalMaterial, &pMutableData);
    return (MutableMaterialWrapper){
        .m_oMaterial = oMaterial,
        .m_pMutableData = pMutableData
    };
}

void DestroyMutableMaterialWrapper(MutableMaterialWrapper* _pMutableMaterialWrapper)
{
    DestroyMaterial(&_pMutableMaterialWrapper->m_oMaterial);
    MEM_free(_pMutableMaterialWrapper->m_pMutableData);
}