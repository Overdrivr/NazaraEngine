// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "QuadTree.hpp"
#include <iostream>

using namespace std;


//#include <Nazara/DynaTerrain/HeightSource.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

NzQuadTree::NzQuadTree(const NzVector2f& terrainCenter, const NzVector2f& terrainSize, NzHeightSource* heightSource)
{
    m_heightSource = heightSource;
    root = new NzNode(this,0,m_heightSource,terrainCenter,terrainSize);
    m_leaves.push_back(root);
    m_nodes.at(0,0,0) = root;
}

NzQuadTree::~NzQuadTree()
{
    root->CleanTree(0);
    cout << "NbNodes non supprimes : "<<root->GetNodeAmount()-1<< endl;
    delete root;
}

void NzQuadTree::DrawTerrain()
{
    //for(unsigned int i(0) ; i < m_leaves.size() ; ++i)
      //  m_leaves.at(i)->Display();
}

const std::list<NzNode*>& NzQuadTree::GetLeavesList()
{
    return m_leaves;
}

NzNode* NzQuadTree::GetNode(id nodeID)
{
    if(m_nodes.Exists(nodeID))
        return m_nodes.at(nodeID);
    else
        return nullptr;
}

NzNode* NzQuadTree::GetRootPtr()
{
    return root;
}

void NzQuadTree::Prepare()
{
    root->SlopeBasedHierarchicalSubdivide(9);
}

void NzQuadTree::RegisterLeaf(NzNode* node)
{
    //Verifier si le node n'y est pas déjà (PB potentiel avec Clean Tree)
    m_leaves.push_back(node);
    m_nodes.at(node->GetNodeID()) = node;
}

bool NzQuadTree::UnRegisterLeaf(NzNode* node)
{
    m_leaves.remove(node);
    //En cas de problèmes, tester si la feuille est bien présente avant de tenter de l'enlever
    return true;
}

bool NzQuadTree::UnRegisterNode(NzNode* node)
{
    if(m_nodes.Exists(node->GetNodeID()))
    {
        m_nodes.Erase(node->GetNodeID());
        return true;
    }
    else
    {
        cout<< "EXCEPTION : NzQuadTree::UnRegisterLeaf : Trying to remove unexisting node"<<node->GetNodeID().lvl<<"|"
                                                                                          <<node->GetNodeID().sx<<"|"
                                                                                          <<node->GetNodeID().sy<<endl;
        return false;
    }
}

void NzQuadTree::Update(const NzVector3d& cameraPosition)
{

}

