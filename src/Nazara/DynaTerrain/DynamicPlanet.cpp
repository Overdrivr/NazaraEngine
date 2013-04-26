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
    for (auto& it: quadtrees)
        delete it;
}

void NzDynamicPlanet::Draw() const
{
    NzDynaTerrainMainClassBase::Draw();
    quadtrees.at(0)->Render();
    quadtrees.at(1)->Render();
    quadtrees.at(2)->Render();
    quadtrees.at(3)->Render();
    //quadtrees.at(4).Render();
    //quadtrees.at(5).Render();
}

void NzDynamicPlanet::Initialize()
{
    NzDynaTerrainMainClassBase::Initialize(static_cast<NzDynaTerrainConfigurationBase>(m_configuration));

    //FIXME : Construire l'index buffer, en coordination avec le dispatcher
    NzDynaTerrainMainClassBase::CreateIndexBuffer(256);

    quadtrees.emplace_back(new NzTerrainQuadTree(m_configuration,m_heightSource));
    quadtrees.at(0)->Initialize();
    quadtrees.emplace_back(new NzTerrainQuadTree(m_configuration,m_heightSource,NzEulerAnglesf(0.f,0.f,-90.f)));
    quadtrees.at(1)->Initialize();
    quadtrees.emplace_back(new NzTerrainQuadTree(m_configuration,m_heightSource,NzEulerAnglesf(0.f,0.f,-180.f)));
    quadtrees.at(2)->Initialize();
    quadtrees.emplace_back(new NzTerrainQuadTree(m_configuration,m_heightSource,NzEulerAnglesf(0.f,0.f,90.f)));
    quadtrees.at(3)->Initialize();

    quadtrees.at(0)->ConnectNeighbor(quadtrees.at(1),RIGHT);
    quadtrees.at(1)->ConnectNeighbor(quadtrees.at(2),RIGHT);
    quadtrees.at(2)->ConnectNeighbor(quadtrees.at(3),RIGHT);
    quadtrees.at(3)->ConnectNeighbor(quadtrees.at(0),RIGHT);
}

void NzDynamicPlanet::Update(const NzVector3f& cameraPosition)
{
     //On transforme la position de la caméra du repère global dans le repère local
    NzVector3f localCamPos = cameraPosition - this->GetPosition();

    quadtrees.at(0)->Update(localCamPos);
    quadtrees.at(1)->Update(localCamPos);
    quadtrees.at(2)->Update(localCamPos);
    quadtrees.at(3)->Update(localCamPos);
    //quadtrees.at(4).Update(localCamPos);
    //quadtrees.at(5).Update(localCamPos);
}
