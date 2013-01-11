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

void NzTerrainQuadTree::Initialize(const NzVector3f& cameraPosition)
{
     m_isInitialized = true;

    //On subdivise l'arbre équitablement au niveau minimum
    m_root->HierarchicalSubdivide(m_configuration.minimumDepth);

    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    //m_root->SlopeBasedHierarchicalSubdivide(m_configuration.slopeMaxDepth);

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
    //On prend un rayon de 200.f dans un premier temps
    //On ajuste en fonction de l'altitude, si la caméra est à plus de 200.f du sol, le rayon est nul, et le périmètre est ignoré
    if(std::fabs(cameraPosition.y) > 200.f)
        return;

    //Une optimisation potentielle à mettre en oeuvre : Au lieu de tester l'ensemble de l'arbre contre le périmètre caméra
    //On teste d'abord l'ensemble de l'arbre sur le périmètre le plus grand
    //On teste ensuite les nodes retenus sur le périmètre suivant (et donc plus petit) et ainsi de suite,
    //On a donc de moins en moins de nodes à tester à chaque fois, mais alors plus d'aspect hiérarchique
    //Les nodes feuilles seront tous testés dés le second périmètre.
    //Cette optimisation sera efficace si traverser l'arbre est plus lent que tester un node contre un périmètre
    //Ce qui est peut probable, mais à tester quand même

    float radius = 200.f - std::fabs(cameraPosition.y);
    NzCirclef cameraRadius(cameraPosition.x,cameraPosition.z,radius);

    m_cameraListAdded.empty();

    //A chaque frame, on recalcule quels noeuds sont dans le périmètre de la caméra
    m_root->HierarchicalAddToCameraList(cameraRadius);

    //Si ils viennent d'être ajoutés, on tente une subdivision
    std::map<id,NzTerrainNode*>::iterator it;

    for(it  = m_cameraListAdded.begin() ; it != m_cameraListAdded.end() ; ++it)
    {
        it->second->HierarchicalSubdivide(6);
    }

    //Les noeuds manquants ont quitté le périmètre, on tente de les refiner
}

void NzTerrainQuadTree::AddLeaveToCameraList(NzTerrainNode* node)
{
    //Le node n'était pas présent à la frame précédente
    if(m_cameraList.count(node->GetNodeID()) == 0)
    {
        m_cameraList[node->GetNodeID()] = node;
        m_cameraListAdded[node->GetNodeID()] = node;
    }
    //Sinon il était déjà dedans, pas besoin de lui refaire subir les opérations
}
