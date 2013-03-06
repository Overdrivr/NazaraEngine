// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/DynamicPlanet.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzDynamicPlanet::NzDynamicPlanet(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource)
{
   if(configuration.IsValid())
        m_configuration = configuration;

    m_heightSource = heightSource;
}

NzDynamicPlanet::~NzDynamicPlanet()
{
    delete quadtree;
    delete quadtree2;
}

void NzDynamicPlanet::Render()
{
    NzRenderer::SetShader(&m_shader);
    quadtree->Render();
    quadtree2->Render();
}

void NzDynamicPlanet::Initialize()
{
    SetShaders(m_configuration.vertexShader,m_configuration.fragmentShader);

    if(!m_terrainTexture.LoadFromFile(m_configuration.terrainTexture))
        std::cout<<"Could not load texture "<<m_configuration.terrainTexture<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);

    int i = m_shader.GetUniformLocation("terrainTexture");

    if(i == -1)
        std::cout<<"Could not retrieve uniform location"<<std::endl;

    m_shader.SendTexture(i,&m_terrainTexture);

    float radius = 2000.f;

    m_configuration.terrainCenter += NzVector3f(0.f,radius,0.f);

    quadtree = new NzPlanetQuadTree(m_configuration,m_heightSource);
    quadtree->Initialize();

    NzTerrainConfiguration second = m_configuration;

    second.terrainCenter += NzVector3f(m_configuration.terrainSize/2.f,-m_configuration.terrainSize/2.f,0.f);
    second.terrainOrientation.roll = -90.f;


    quadtree2 = new NzPlanetQuadTree(second,m_heightSource);
    quadtree2->Initialize();

    quadtree->ConnectNeighbor(quadtree2,RIGHT);
}

bool NzDynamicPlanet::SetShaders(const NzString& vertexShader, const NzString& fragmentShader)
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

void NzDynamicPlanet::Update(const NzVector3f& cameraPosition)
{
    quadtree->Update(cameraPosition);
    quadtree2->Update(cameraPosition);
}
