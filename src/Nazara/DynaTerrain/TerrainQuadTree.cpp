// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "TerrainQuadTree.hpp"
#include <iostream>
#include <cmath>

using namespace std;

//#include <Nazara/DynaTerrain/HeightSource.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainQuadTree::NzTerrainQuadTree(const NzTerrainQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource)
{

    if(configuration.IsValid())
        m_configuration = configuration;

    m_heightSource = heightSource;
    m_data.quadtree = this;
    m_data.heightSource = m_heightSource;
    m_data.dispatcher = new NzDispatcher;

    m_buffersAmount = 1+(600*(m_configuration.ComputeMaxPatchNumber()))/1048576;
    cout<<"1 Mio buffers amount : "<<m_buffersAmount<<endl;

    m_data.dispatcher->Initialize(m_configuration.minimumDepth,m_buffersAmount);

    root = new NzTerrainNode(&m_data,0,terrainCenter,NzVector2f(m_configuration.terrainSize,m_configuration.terrainSize));
    m_leaves.push_back(root);
    m_nodes.at(0,0,0) = root;

    m_isInitialized = false;
}

NzTerrainQuadTree::~NzTerrainQuadTree()
{
    root->CleanTree(0);
    delete m_data.dispatcher;
    cout << "NbNodes non supprimes : "<<root->GetNodeAmount()-1<< endl;
    delete root;
}

void NzTerrainQuadTree::Render()
{
    m_data.dispatcher->DrawAll();
}

const std::list<NzTerrainNode*>& NzTerrainQuadTree::GetLeavesList()
{
    return m_leaves;
}

NzTerrainNode* NzTerrainQuadTree::GetNode(id nodeID)
{
    if(m_nodes.Exists(nodeID))
        return m_nodes.at(nodeID);
    else
        return nullptr;
}

float NzTerrainQuadTree::GetMaximumHeight() const
{
    return m_configuration.terrainHeight;
}

NzTerrainNode* NzTerrainQuadTree::GetRootPtr()
{
    return root;
}

void NzTerrainQuadTree::Initialize(const NzVector3f& cameraPosition)
{
     m_isInitialized = true;

    //On subdivise l'arbre équitablement au niveau minimum
    root->HierarchicalSubdivide(m_configuration.minimumDepth);

    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    root->SlopeBasedHierarchicalSubdivide(m_configuration.slopeMaxDepth);

    //La partie statique de l'arbre est prête
    //L'arbre ne pourra plus être refiné en dessous des niveaux définits à ce stade

    //Si la contribution proche de la camera n'est pas 0 (= pas d'optimisations), on prépare le terrain pour la camera
    //if(closeCameraContribution > 0)
        //this->Update(cameraPosition);

    //On demander au dispatcher d'allouer les buffers nécessaires
    //m_buffersAmount


}

void NzTerrainQuadTree::RegisterLeaf(NzTerrainNode* node)
{
    //Verifier si la cellule n'y est pas déjà (PB potentiel avec Clean Tree)
    m_leaves.push_back(node);
    m_nodes.at(node->GetNodeID()) = node;
}

bool NzTerrainQuadTree::UnRegisterLeaf(NzTerrainNode* node)
{
    m_leaves.remove(node);
    //En cas de problèmes, tester si la feuille est bien présente avant de tenter de l'enlever
    return true;
}

bool NzTerrainQuadTree::UnRegisterNode(NzTerrainNode* node)
{
    if(m_nodes.Exists(node->GetNodeID()))
    {
        m_nodes.Erase(node->GetNodeID());
        return true;
    }
    else
    {
        cout<< "EXCEPTION : NzTerrainQuadTree::UnRegisterLeaf : Trying to remove unexisting node"<<node->GetNodeID().lvl<<"|"
                                                                                                 <<node->GetNodeID().sx<<"|"
                                                                                                 <<node->GetNodeID().sy<<endl;
        return false;
    }
}

void NzTerrainQuadTree::Update(const NzVector3d& cameraPosition)
{

}



