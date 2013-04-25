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

    quadtree = new NzTerrainQuadTree(m_configuration,m_heightSource);
    quadtree->Initialize();
    quadtree2 = new NzTerrainQuadTree(m_configuration,m_heightSource,NzEulerAnglesf(0.f,0.f,-90.f));
    quadtree2->Initialize();

    quadtree->ConnectNeighbor(quadtree2,RIGHT);
}

void NzDynamicPlanet::Update(const NzVector3f& cameraPosition)
{
     //On transforme la position de la caméra du repère global dans le repère local
    NzVector3f localCamPos = cameraPosition - this->GetPosition();

    quadtree->Update(localCamPos);
    quadtree2->Update(localCamPos);
}
