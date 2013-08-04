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
    m_faceFilling = nzFaceFilling_Fill;
}

NzTerrainBase::~NzTerrainBase()
{
    if(m_shader)
        delete m_shader;
    m_shader = nullptr;
    delete m_indexBuffer;
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
    if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

    nzUInt8 textureUnit;
	m_shader->SendTexture(m_shader->GetUniformLocation("terrainTexture"), &m_terrainTexture, &textureUnit);

    NzRenderer::SetMatrix(nzMatrixType_World,m_transformMatrix);
    NzRenderer::SetFaceCulling(nzFaceCulling_Back);
    NzRenderer::SetFaceFilling(m_faceFilling);
    NzRenderer::Enable(nzRendererParameter_FaceCulling, false);

    //NzRenderer::Enable(nzRendererParameter_Blend, false);
    NzRenderer::Enable(nzRendererParameter_DepthTest, true);
    //NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
    NzRenderer::SetShader(m_shader);
    NzRenderer::SetTextureSampler(textureUnit, m_sampler);
    NzRenderer::SetIndexBuffer(m_indexBuffer);
}

void NzTerrainBase::Initialize(const NzDynaTerrainConfigurationBase& configuration)
{
    BuildShader();

    if(!m_terrainTexture.LoadFromFile(configuration.groundTextures))
        std::cout<<"Could not load texture "<<configuration.groundTextures<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);


    float radius = configuration.higherCameraPrecisionRadius;
    for(int i(0) ; i < configuration.cameraRadiusAmount ; ++i)
    {
        std::cout<<"radius "<<configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= configuration.radiusSizeIncrement;
    }

     m_sampler.SetWrapMode(nzSamplerWrap_Repeat);
}

void NzTerrainBase::SetFaceFilling(const nzFaceFilling& faceFilling)
{
    m_faceFilling = faceFilling;
}

void NzTerrainsBase::Update(const NzVector3f& cameraPosition)
{

}

bool NzTerrainBase::VisibilityTest(const NzFrustumf& frustum)
{
    //FIX ME
    return true;
}
