// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainBase.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzTerrainBase::NzTerrainBase()
{

}

nzSceneNodeType NzTerrainBase::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}

void NzTerrainBase::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
    renderQueue->AddDrawable(this);
}

bool NzTerrainBase::VisibilityTest(const NzCamera* camera)
{
    // A bouger dans GetBoundingVolume si cette ligne est nécessaire
    //GetBoundingVolume().Update(NzMatrix4f::Identity());

    return camera->GetFrustum().Intersect(GetBoundingVolume()) != nzIntersectionSide_Outside;
}

//void NzTerrainBase::Initialize(const NzDynaTerrainConfigurationBase& configuration)
//{
    //FIXME : BOUGER DANS LES CLASSES FILLES ?
    //if(!m_terrainTexture.LoadFromFile(configuration.groundTextures))
    //    std::cout<<"Could not load texture "<<configuration.groundTextures<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);

     //m_sampler.SetWrapMode(nzSamplerWrap_Repeat);
//}

