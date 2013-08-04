// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainBase.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzTerrainBase::NzTerrainBase()
{

}

void NzTerrainBase::AddToRenderQueue(NzForwardRenderQueue& renderQueue) const
{
    renderQueue.AddDrawable(this);
}

const NzBoundingVolumef& NzTerrainBase::GetBoundingBox() const
{
    return m_aabb;
}

nzSceneNodeType NzTerrainBase::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}

//void NzTerrainBase::Initialize(const NzDynaTerrainConfigurationBase& configuration)
//{
    //if(!m_terrainTexture.LoadFromFile(configuration.groundTextures))
    //    std::cout<<"Could not load texture "<<configuration.groundTextures<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);

    //TODO : A BOUGER DANS NzDynaTerrain, d'ailleurs la configuration de la camera devrait se faire là,
    // pas pour chaque terrain
    /*float radius = configuration.higherCameraPrecisionRadius;
    for(int i(0) ; i < configuration.cameraRadiusAmount ; ++i)
    {
        std::cout<<"radius "<<configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= configuration.radiusSizeIncrement;
    }*/

     //m_sampler.SetWrapMode(nzSamplerWrap_Repeat);
//}

void NzTerrainBase::Update(const NzVector3f& cameraPosition)
{

}
