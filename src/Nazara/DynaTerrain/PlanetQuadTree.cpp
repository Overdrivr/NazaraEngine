// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/PlanetQuadTree.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

NzPlanetQuadTree::NzPlanetQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource) :
NzTerrainQuadTree(configuration,heightSource)
{

}

NzPlanetQuadTree::~NzPlanetQuadTree()
{
    std::cout<<"Maximum amount of operations per frame : "<<m_maxOperationsPerFrame<<std::endl;
    std::cout<<"Libération de "<<m_nodesPool.GetPoolSize()<<" node(s), veuillez patientez..."<<std::endl;
    NzClock clk;
    clk.Restart();
    m_nodesPool.ReleaseAll();
    m_patchesPool.ReleaseAll();
    clk.Pause();
    std::cout<<"Arbre libere en "<<clk.GetMilliseconds()/1000.f<<" s "<<std::endl;
    std::cout<<"NbNodes non supprimes : "<<m_root->GetNodeAmount()<<std::endl;
}

NzVector3f NzPlanetQuadTree::GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y)
{
    NzVector3f position;

    //Les coordonnées d'un plan
    position.x = m_configuration.terrainSize * (x * 0.25f + nodeID.locx) / std::pow(2,nodeID.depth) - m_configuration.terrainSize/2.f;
    position.y = 0.f;
    position.z = m_configuration.terrainSize * (y * 0.25f + nodeID.locy) / std::pow(2,nodeID.depth) - m_configuration.terrainSize/2.f;

    //On l'oriente correctement
    position = m_rotationMatrix.Transform(position);

    position += m_configuration.terrainCenter;

    //On normalise le vecteur pour obtenir une sphère
    position.Normalize();
    position *= 2000.f;

    //Et applique la hauteur en cette position
    //position *= m_heightSource3->GetHeight(position.x, position.y, position.z) * m_configuration.maxTerrainHeight;

    return position;
}
