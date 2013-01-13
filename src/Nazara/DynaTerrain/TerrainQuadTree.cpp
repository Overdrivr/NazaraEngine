// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/String.hpp>
#include "TerrainQuadTree.hpp"
#include <Nazara/Math/Circle.hpp>
#include <Nazara/Math/Rect.hpp>
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

    m_root = new NzTerrainNode(&m_data,0,terrainCenter,NzVector2f(m_configuration.terrainSize,m_configuration.terrainSize));
    m_leaves.push_back(m_root);
    m_nodes[id(0,0,0)] = m_root;

    m_isInitialized = false;

    m_currentCameraRadiusIndex = 0;
}

NzTerrainQuadTree::~NzTerrainQuadTree()
{
    m_root->CleanTree(0);
    delete m_data.dispatcher;
    cout << "NbNodes non supprimes : "<<m_root->GetNodeAmount()-1<< endl;
    delete m_root;
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
    if(m_nodes.count(nodeID) == 1)
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
    return m_root;
}

unsigned int NzTerrainQuadTree::GetUpdatedNodeAmountPerFrame() const
{
    return m_subdivideList.size();
}

void NzTerrainQuadTree::Initialize(const NzVector3f& cameraPosition)
{
     m_isInitialized = true;

    //On subdivise l'arbre équitablement au niveau minimum
    m_root->HierarchicalSubdivide(m_configuration.minimumDepth);

    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    m_root->SlopeBasedHierarchicalSubdivide(m_configuration.slopeMaxDepth);

    //La partie statique de l'arbre est prête
    //L'arbre ne pourra plus être refiné en dessous des niveaux définits à ce stade

    //Si la contribution proche de la camera n'est pas 0 (= pas d'optimisations), on prépare le terrain pour la camera
    //if(closeCameraContribution > 0)
        //this->Update(cameraPosition);


}

void NzTerrainQuadTree::RegisterLeaf(NzTerrainNode* node)
{
    if(m_nodes.count(node->GetNodeID()) == 0)
    {
        m_leaves.push_back(node);
        m_nodes[node->GetNodeID()] = node;
    }
}

bool NzTerrainQuadTree::UnRegisterLeaf(NzTerrainNode* node)
{
    m_leaves.remove(node);

    //Enlever de la liste de caméra un node si il y est déjà ??
    return true;
}

bool NzTerrainQuadTree::UnRegisterNode(NzTerrainNode* node)
{
    std::map<id,NzTerrainNode*>::iterator it = m_nodes.find(node->GetNodeID());
    if(it != m_nodes.end())
    {
        m_nodes.erase(it);
        return true;
    }
    else
    {
        NazaraError("NzTerrainQuadTree::UnRegisterLeaf : Trying to remove unexisting node" +
                    NzString::Number(node->GetNodeID().lvl) + "|" +
                    NzString::Number(node->GetNodeID().sx) + "|" +
                    NzString::Number(node->GetNodeID().sy));
        return false;
    }
}

void NzTerrainQuadTree::Update(const NzVector3f& cameraPosition)
{
    /*
    NzVector3f centerPoint(cameraPosition.x,cameraPosition.z,cameraPosition.y);
    m_root->HierarchicalDynamicPrecisionAroundPoint(centerPoint);*/

    float radius = 200.f;
    //On ajuste en fonction de l'altitude, si la caméra est à plus de 200.f du sol, le rayon est nul, et le périmètre est ignoré
    //if(std::fabs(cameraPosition.y) > radius-20.f)

    //Une optimisation potentielle à mettre en oeuvre : Au lieu de tester l'ensemble de l'arbre contre le périmètre caméra
    //On teste d'abord l'ensemble de l'arbre sur le périmètre le plus grand
    //On teste ensuite les nodes retenus sur le périmètre suivant (et donc plus petit) et ainsi de suite,
    //On a donc de moins en moins de nodes à tester à chaque fois, mais alors plus d'aspect hiérarchique
    //Les nodes feuilles seront tous testés dés le second périmètre.
    //Cette optimisation sera efficace si traverser l'arbre est plus lent que tester un node contre un périmètre
    //Ce qui est peut probable, mais à tester quand même

    //radius -= std::fabs(cameraPosition.y);
    NzCirclef cameraRadius(cameraPosition.x,cameraPosition.z,radius);

    m_subdivideList.clear();
    m_removeList.clear();

    //A chaque frame, on recalcule quels noeuds sont dans le périmètre de la caméra
    m_root->HierarchicalAddToCameraList(cameraRadius,7);

    /*if(!m_subdivideList.empty())
        std::cout<<"Subdivisions amount : "<<m_subdivideList.size()<<std::endl;*/
    /*if(!m_removeList.empty())
        std::cout<<"Refines amount : "<<m_removeList.size()<<std::endl;*/

    //On subdivise les nodes nécessaires
    std::map<id,NzTerrainNode*>::iterator it;

    for(it = m_subdivideList.begin() ; it != m_subdivideList.end() ; ++it)
    {
        it->second->Subdivide();
    }

    //On refine les nodes nécessaires
    /*for(it = m_removeList.begin() ; it != m_removeList.end() ; ++it)
    {
        it->second->Refine();
    }*/

    //Note : cette méthode ne subdivisera pas l'arbre dans la position optimale de caméra dés la première frame
    //A chaque frame, un seul niveau sera subdivisé, ce qui étalera le travail sur plusieurs frames

    //Les noeuds manquants ont quitté le périmètre, on tente de les refiner

    ///Autre méthode
    //Chaque node détermine grâce à la position de la caméra :
        //A quel rayon il appartient
            //Si il est dans un rayon et que son niveau est trop important
                //Il demande un refine
            //Si il est dans un rayon et que son niveau est trop faible
                //Il demande un subdivide
            //Si il coupe un rayon, on teste ses enfants contre les mêmes conditions
            //Si il ne remplit aucune des 3 conditions, on s'arrête là
    ///Avantage : une seule traversée de l'arbre
    ///Peut également mettre à jour l'arbre avec la variation de pente dynamique
    ///Inconvénient, l'ensemble des nodes contenus dans un rayon de la caméra seront traversés jusqu'en bas de l'arbre

}

void NzTerrainQuadTree::AddLeaveToSubdivisionList(NzTerrainNode* node)
{
    m_subdivideList[node->GetNodeID()] = node;
}

int NzTerrainQuadTree::TransformDistanceToCameraInRadiusIndex(float distance)
{
    std::cout<<distance<<std::endl;
    if(distance > m_configuration.startRadius + m_configuration.effectRadius)
    {
        return -1;
    }
    else if(distance < m_configuration.startRadius)
    {
        return m_configuration.closeCameraDepth;
    }
    else
    {
        float pas = m_configuration.effectRadius/(m_configuration.closeCameraDepth - m_configuration.farCameraDepth);
        return m_configuration.closeCameraDepth - 1 + static_cast<unsigned int>((distance-m_configuration.startRadius)/pas);
    }
}
