// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/DynamicTerrain.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzDynamicTerrain::NzDynamicTerrain(const NzTerrainConfiguration& configuration, NzHeightSource2D* heightSource)
{
   if(configuration.IsValid())
        m_configuration = configuration;

    m_heightSource = heightSource;
}

NzDynamicTerrain::~NzDynamicTerrain()
{
    delete quadtree;
    //delete quadtree2;
}

void NzDynamicTerrain::Draw() const
{
    NzDynaTerrainMainClassBase::Draw();
    quadtree->Render();
    //quadtree2->Render();
}

void NzDynamicTerrain::Initialize()
{
    NzDynaTerrainMainClassBase::Initialize(static_cast<NzDynaTerrainConfigurationBase>(m_configuration));

    //FIXME : Construire l'index buffer, en coordination avec le dispatcher
    NzDynaTerrainMainClassBase::CreateIndexBuffer(256);

    quadtree = new NzTerrainQuadTree(m_configuration,m_heightSource);
    quadtree->Initialize();

    /*NzTerrainConfiguration second = m_configuration;

    second.center += NzVector3f(m_configuration.terrainSize,0.f,0.f);
   //second.terrainOrientation.roll = 90.f;


    quadtree2 = new NzDynaTerrainQuadTreeBase(second,m_heightSource);
    quadtree2->Initialize();

    quadtree->ConnectNeighbor(quadtree2,RIGHT);*/
}

void NzDynamicTerrain::Update(const NzVector3f& cameraPosition)
{
    quadtree->Update(cameraPosition);
    //quadtree2->Update(cameraPosition);
}
