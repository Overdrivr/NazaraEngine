// Copyright (C) 2012 Rémi Bèges
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

NzQuadTree::NzQuadTree(const NzVector2f& terrainCenter, const NzVector2f& terrainSize, NzHeightSource* heightSource)
{
    m_heightSource = heightSource;
    root = new NzNode(this,0,m_heightSource,terrainCenter,terrainSize);
    m_leaves.push_back(root);
    m_nodes.at(0,0,0) = root;
    m_isInitialized = false;

    m_minimumDepth = 0;
    m_slopeContribution = 0;
    m_closeCameraContribution = 0;
    m_farCameraContribution = 0;
    m_distanceContribution = 1.f;
    m_terrainSize = terrainSize;
}

NzQuadTree::~NzQuadTree()
{
    root->CleanTree(0);
    cout << "NbNodes non supprimes : "<<root->GetNodeAmount()-1<< endl;
    delete root;
}

bool NzQuadTree::ComputeGPUBuffersAmount()
{
    if(!m_isInitialized)
        return false;

    unsigned int camera = 0;

    //Pour chaque cercle de changement de profondeur
    //i = 0 : plus grand cercle
    //i = m_closeCameraContribution-m_farCameraContribution-1 : plus petit

    unsigned int nbCercles = m_closeCameraContribution-m_farCameraContribution+1;

    for(unsigned int i(0) ; i < nbCercles ; ++i)
    {
        if(i > nbCercles-1)
        {

            float radius = m_distanceContribution/nbCercles;
            float edge = m_terrainSize.x/(std::pow(2,m_closeCameraContribution));
            //On calcule le nombre de patchs dans le cercle le plus petit
            camera += static_cast<unsigned int>(2*radius/edge);
        }
        else
        {
            float radius = m_distanceContribution*(nbCercles-i)/nbCercles;

            float radius2 = m_distanceContribution*(nbCercles-i-1)/nbCercles;

            float edge = m_terrainSize.x/(std::pow(2,m_farCameraContribution+i));
            //On calcule le nombre de patchs entre le cercle i et le cercle i+1
            camera += static_cast<unsigned int>(2*radius/edge) - static_cast<unsigned int>(2*radius2/edge);
        }

    }

    float edge = m_terrainSize.x/(std::pow(2,2*(m_farCameraContribution)));
    unsigned int slope = static_cast<unsigned int>(std::pow(2,2*m_slopeContribution)) - static_cast<unsigned int>(2*m_distanceContribution/edge);

    //1 patch pèse 800 octets et on veut des buffers de 1Mio, soit 1 048 576 octets
    m_buffersAmount = 1+(600*(camera + slope))/1048576;

    //Dans le cas extrême où l'ensemble des patchs ne parvient même pas à remplir un buffer, le culling sera une perte de temps
    //mais le terrain sera tellement peu précis que faire du culling est de toute façon inutile

    cout<<"buffers amount : "<<m_buffersAmount<<" Mio | camera : "<<(600*camera)/1048576<<" Mio | slope : "<<(600*slope)/1048576<<" Mio"<<endl;
    return true;
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

void NzQuadTree::Initialize(unsigned int minimumDepth, unsigned int slopeContribution, unsigned int closeCameraContribution, unsigned int farCameraContribution, float distanceContribution, const NzVector3f& cameraPosition)
{
     m_isInitialized = true;
    //FIX ME : vérifier l'ordre des >

    //On subdivise l'arbre équitablement au niveau minimum
    //root->HierarchicalSubdivide(minimumDepth);

    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    //if(slopeContribution > 0)
        //root->SlopeBasedHierarchicalSubdivide(slopeContribution);

    //La partie statique de l'arbre est prête
    //L'arbre ne pourra plus être refiné en dessous des niveaux définits à ce stade

    //Si la contribution proche de la camera n'est pas 0 (= pas d'optimisations), on prépare le terrain pour la camera
    //if(closeCameraContribution > 0)
        //this->Update(cameraPosition);

    m_minimumDepth = minimumDepth;
    m_slopeContribution = slopeContribution;
    m_closeCameraContribution = closeCameraContribution;
    m_farCameraContribution = farCameraContribution;
    m_distanceContribution = distanceContribution;

    //On calcule le nombre de buffers GPU de 1Mo à allouer
    ComputeGPUBuffersAmount();

    //On demander au dispatcher d'allouer ces buffers
    //m_buffersAmount


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



