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

void NzTerrainBase::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
    renderQueue.otherDrawables.push_back(this);
}

const NzBoundingBoxf& NzTerrainBase::GetBoundingBox() const
{
    return m_aabb;
}

nzSceneNodeType NzTerrainBase::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}

void NzTerrainBase::Draw() const
{
    //A RAJOUTER DANS PLANET !!!
    //if (!m_transformMatrixUpdated)
	//	UpdateTransformMatrix();
    //NzRenderer::SetMatrix(nzMatrixType_World,m_transformMatrix);

    //TODO : A BOUGER !
    //nzUInt8 textureUnit;
	//m_shader->SendTexture(m_shader->GetUniformLocation("terrainTexture"), &m_terrainTexture, &textureUnit);


    //NzRenderer::SetFaceCulling(nzFaceCulling_Back);
    //NzRenderer::SetFaceFilling(m_faceFilling);
    //NzRenderer::Enable(nzRendererParameter_FaceCulling, false);

    //NzRenderer::Enable(nzRendererParameter_Blend, false);
    //NzRenderer::Enable(nzRendererParameter_DepthTest, true);
    //NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
    //NzRenderer::SetShader(m_shader);
    //NzRenderer::SetTextureSampler(textureUnit, m_sampler);
    //NzRenderer::SetIndexBuffer(m_indexBuffer);
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

void NzTerrainsBase::Update(const NzVector3f& cameraPosition)
{

}
