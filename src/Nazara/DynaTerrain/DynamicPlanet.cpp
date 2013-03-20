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

NzDynamicPlanet::NzDynamicPlanet(const NzPlanetConfiguration& configuration, NzHeightSource3D* heightSource)
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

void NzDynamicPlanet::Draw() const
{
    NzDynaTerrainMainClassBase::Draw();
    quadtree->Render();
    quadtree2->Render();
}

void NzDynamicPlanet::Initialize()
{
    NzDynaTerrainMainClassBase::Initialize(static_cast<NzDynaTerrainConfigurationBase>(m_configuration));

    //FIXME : Construire l'index buffer, en coordination avec le dispatcher
    NzDynaTerrainMainClassBase::CreateIndexBuffer(256);

    //float radius = 2000.f;

    //m_configuration.center += NzVector3f(-m_configuration.planetRadius,m_configuration.planetRadius,-m_configuration.planetRadius);
    std::cout<<m_configuration.center<<std::endl;
    quadtree = new NzDynaTerrainQuadTreeBase(m_configuration,m_heightSource);
    quadtree->Initialize();

    NzPlanetConfiguration second = m_configuration;

    //second.center += NzVector3f(0.f,-m_configuration.planetRadius/4.f,0.f);
    std::cout<<second.center<<std::endl;

    quadtree2 = new NzDynaTerrainQuadTreeBase(second,m_heightSource,NzEulerAnglesf(0.f,0.f,-90.f));
    quadtree2->Initialize();

    quadtree->ConnectNeighbor(quadtree2,RIGHT);
}

void NzDynamicPlanet::Update(const NzVector3f& cameraPosition)
{
    quadtree->Update(cameraPosition);
    quadtree2->Update(cameraPosition);
}
