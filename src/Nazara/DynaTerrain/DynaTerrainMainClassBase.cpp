// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/DynaTerrainMainClassBase.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzDynaTerrainMainClassBase::NzDynaTerrainMainClassBase()
{

}

NzDynaTerrainMainClassBase::~NzDynaTerrainMainClassBase()
{
    delete m_indexBuffer;
}

void NzDynaTerrainMainClassBase::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
    renderQueue.otherDrawables.push_back(this);
}

void NzDynaTerrainMainClassBase::CreateIndexBuffer(unsigned int bufferCapacity, bool appendConfigurations)
{
    std::cout<<"Creating index buffer with capacity : "<<bufferCapacity<<std::endl;
    m_bufferCapacity = bufferCapacity;
    //On construit l'index buffer
    //taille totale : 1750 * 96 = 168000
    //taille atomique : 96
    //32 triangles

    //L'index pour deux triangles
    unsigned int rowIndex[24];
    for(int i(0) ; i < 4 ; ++i)
    {
        rowIndex[i*6] = i;
        rowIndex[i*6+1] = i + 6;
        rowIndex[i*6+2] = i + 1;
        rowIndex[i*6+3] = i;
        rowIndex[i*6+4] = i + 5;
        rowIndex[i*6+5] = i + 6;
    }

    //L'index d'un patch entier
    unsigned int indexes[96];
    for(unsigned int i(0) ; i < 4 ; ++i)
        for(unsigned int j(0) ; j < 24 ; ++j)
        {
            indexes[i*24+j] = rowIndex[j] + i*5;
        }

    //L'index buffer en entier contenant "bufferCapacity" patchs
    std::vector<unsigned int> allIndexes;
    allIndexes.reserve(bufferCapacity * 96);
    for(int i(0) ; i < bufferCapacity ; ++i)
        for(int j(0) ; j < 96 ; ++j)
        {
            //allIndexes[i*96+j] = indexes[j] + 25*i;
            allIndexes.push_back(indexes[j] + 25 * i);
        }

	m_indexBuffer = new NzIndexBuffer(bufferCapacity * 96, true, nzBufferStorage_Hardware);
	if (!m_indexBuffer->Fill(allIndexes.data(), 0, bufferCapacity * 96)) // FIX ME : Que faire en cas d'échec
	{
		std::cout << "Failed to fill indexbuffer" << std::endl;
	}
}

const NzBoundingBoxf& NzDynaTerrainMainClassBase::GetBoundingBox() const
{
    return m_aabb;
}

nzSceneNodeType NzDynaTerrainMainClassBase::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}

void NzDynaTerrainMainClassBase::Draw() const
{
    if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

    NzRenderer::SetMatrix(nzMatrixType_World,m_transformMatrix);
    NzRenderer::SetFaceCulling(nzFaceCulling_Back);
    //NzRenderer::Enable(nzRendererParameter_Blend, false);
    NzRenderer::Enable(nzRendererParameter_DepthTest, true);
    //NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
    NzRenderer::SetShader(&m_shader);
    NzRenderer::SetIndexBuffer(m_indexBuffer);
}

void NzDynaTerrainMainClassBase::Initialize(const NzDynaTerrainConfigurationBase& configuration)
{
    SetShaders(configuration.vertexShader,configuration.fragmentShader);

    if(!m_terrainTexture.LoadFromFile(configuration.groundTextures))
        std::cout<<"Could not load texture "<<configuration.groundTextures<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);

    int i = m_shader.GetUniformLocation("terrainTexture");

    if(i == -1)
        std::cout<<"Could not retrieve uniform location"<<std::endl;

    m_shader.SendTexture(i,&m_terrainTexture);


    float radius = configuration.higherCameraPrecisionRadius;
    for(int i(0) ; i < configuration.cameraRadiusAmount ; ++i)
    {
        std::cout<<"radius "<<configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= configuration.radiusSizeIncrement;
    }
}

bool NzDynaTerrainMainClassBase::SetShaders(const NzString& vertexShader, const NzString& fragmentShader)
{
    if (!m_shader.Create(nzShaderLanguage_GLSL))
    {
        std::cout << "Failed to load shader" << std::endl;
        std::getchar();
        return false;
    }

    if (!m_shader.LoadFromFile(nzShaderType_Fragment, fragmentShader))
    {
        std::cout << "Failed to load fragment shader from file" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    if (!m_shader.LoadFromFile(nzShaderType_Vertex, vertexShader))
    {
        std::cout << "Failed to load vertex shader from file" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    if (!m_shader.Compile())
    {
        std::cout << "Failed to compile shader" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    return true;
}

void NzDynaTerrainMainClassBase::Update(const NzVector3f& cameraPosition)
{

}

bool NzDynaTerrainMainClassBase::VisibilityTest(const NzFrustumf& frustum)
{
    //FIX ME
    return true;
}
