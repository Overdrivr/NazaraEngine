// Copyright (C) 2012 R�mi B�ges
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

    //m_configuration.x_offset = 0;
    //m_configuration.y_offset = 0;
    quadtree = new NzTerrainQuadTree(m_configuration,m_heightSource);
    quadtree->Initialize();

    /*NzTerrainConfiguration second = m_configuration;
    second.x_offset = 0;
    second.y_offset = 0;
   //second.terrainOrientation.roll = 90.f;
    quadtree2 = new NzDynaTerrainQuadTreeBase(second,m_heightSource);
    quadtree2->Initialize();

    quadtree->ConnectNeighbor(quadtree2,RIGHT);*/
}

void NzDynamicTerrain::Update(const NzVector3f& cameraPosition)
{
    //On transforme la position de la cam�ra du rep�re global dans le rep�re local
    NzVector3f localCamPos = cameraPosition - this->GetPosition();

    quadtree->Update(localCamPos);
    //quadtree2->Update(cameraPosition);
}