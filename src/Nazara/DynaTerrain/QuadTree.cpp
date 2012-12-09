// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "QuadTree.hpp"
#include <iostream>
#include <cmath>

using namespace std;


//#include <Nazara/DynaTerrain/HeightSource.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

NzQuadTree::NzQuadTree(const NzQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource)
{

    if(configuration.IsValid())
        m_configuration = configuration;

    m_heightSource = heightSource;
    root = new NzNode(this,0,m_heightSource,terrainCenter,NzVector2f(m_configuration.terrainSize,m_configuration.terrainSize));
    m_leaves.push_back(root);
    m_nodes.at(0,0,0) = root;

    m_buffersAmount = 1+(600*(m_configuration.ComputeMaxPatchNumber()))/1048576;
    cout<<"1 Mio buffers amount : "<<m_buffersAmount<<endl;

    m_isInitialized = false;
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

void NzQuadTree::Initialize(const NzVector3f& cameraPosition)
{
     m_isInitialized = true;

    //On subdivise l'arbre �quitablement au niveau minimum
    root->HierarchicalSubdivide(m_configuration.minimumDepth);

    //Si on doit am�liorer l'arbre l� o� la pente est la plus forte, on le fait �galement
    root->SlopeBasedHierarchicalSubdivide(m_configuration.slopeMaxDepth);

    //La partie statique de l'arbre est pr�te
    //L'arbre ne pourra plus �tre refin� en dessous des niveaux d�finits � ce stade

    //Si la contribution proche de la camera n'est pas 0 (= pas d'optimisations), on pr�pare le terrain pour la camera
    //if(closeCameraContribution > 0)
        //this->Update(cameraPosition);

    //On demander au dispatcher d'allouer les buffers n�cessaires
    //m_buffersAmount


}

void NzQuadTree::RegisterLeaf(NzNode* node)
{
    //Verifier si le node n'y est pas d�j� (PB potentiel avec Clean Tree)
    m_leaves.push_back(node);
    m_nodes.at(node->GetNodeID()) = node;
}

bool NzQuadTree::UnRegisterLeaf(NzNode* node)
{
    m_leaves.remove(node);
    //En cas de probl�mes, tester si la feuille est bien pr�sente avant de tenter de l'enlever
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



