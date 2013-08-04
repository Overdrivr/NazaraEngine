// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <cmath>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzTerrainQuadTree::NzTerrainQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource2D* heightSource)
{
    m_type = TERRAIN;

    m_heightSource2D = heightSource;
    m_terrainConfiguration = configuration;
    m_halfTerrainSize = m_terrainConfiguration.terrainSize / 2.f;
    m_commonConfiguration = static_cast<NzDynaTerrainConfigurationBase>(configuration);

    m_isInitialized = false;

    m_rotationMatrix.MakeRotation(NzQuaternionf(m_terrainConfiguration.terrainOrientation));

    Construct();
}

NzTerrainQuadTree::NzTerrainQuadTree(const NzPlanetConfiguration& configuration, NzHeightSource3D* heightSource, const NzEulerAnglesf& quadtreeOrientation)
{
    m_type = PLANET;

    m_heightSource3D = heightSource;
    m_planetConfiguration = configuration;
    //m_halfTerrainSize = m_planetConfiguration.terrainSize / 2.f;
    m_commonConfiguration = static_cast<NzDynaTerrainConfigurationBase>(configuration);

    m_rotationMatrix.MakeRotation(NzQuaternionf(quadtreeOrientation));

    m_isInitialized = false;

    Construct();
}

void NzTerrainQuadTree::Construct()
{
    m_nodesPool.SetChunkSize(5000);
    m_patchesPool.SetChunkSize(1000);
    m_verticesPool.SetChunkSize(1000);

    m_dispatcher.Initialize(m_commonConfiguration.minPrecision);

    m_data.quadtree = this;
    m_data.dispatcher = &m_dispatcher;

    m_root = m_nodesPool.GetObjectPtr();
    m_root->Initialize(&m_data,nullptr);

    m_leaves.push_back(m_root);//I
    m_nodesMap[NzTerrainNodeID(0,0,0)] = m_root;

    m_subdivisionsAmount = 0;

    m_neighbours[0] = nullptr;
    m_neighbours[1] = nullptr;
    m_neighbours[2] = nullptr;
    m_neighbours[3] = nullptr;

    m_poolReallocationSize = 200;
    m_poolAllocatedSpace = 0;
    m_maxOperationsPerFrame = 0;

}

NzTerrainQuadTree::~NzTerrainQuadTree()
{
    //cout<<"Maximum amount of operations per frame : "<<m_maxOperationsPerFrame<<std::endl;
    cout<<"Libération de "<<m_nodesPool.GetPoolSize()<<" node(s)."<<endl;
    NzClock clk;
    clk.Restart();
    m_nodesPool.ReleaseAll();
    m_patchesPool.ReleaseAll();
    clk.Pause();
    //cout<<"Arbre libere en "<<clk.GetMilliseconds()/1000.f<<" s "<<endl;
    //cout<<"NbNodes non supprimes : "<<m_root->GetNodeAmount()<< endl;
    m_isInitialized = false;
}

void NzTerrainQuadTree::ConnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection callerDirection, nzDirection calleeDirection)
{
    m_neighbours[callerDirection] = neighbour;
    neighbour->m_neighbours[calleeDirection] = this;

    nzConnectionType connection;

    if(callerDirection == calleeDirection)
        connection = nzConnectionType_reverse;
    else if(std::abs(callerDirection - calleeDirection) == 1)
    {
        //|TOP - BOTTOM| = 1
        //|LEFT - RIGHT| = 1
        connection = nzConnectionType_straight;
    }
    else
        connection = nzConnectionType_orthogonal;

    m_connectionType[neighbour] = connection;
    neighbour->m_connectionType[this] = connection;

    m_connectionDirectionLookup[neighbour] = callerDirection;
    neighbour->m_connectionDirectionLookup[this] = calleeDirection;
}

void NzTerrainQuadTree::DisconnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection direction)
{
    nzDirection invDirection = direction;
    switch(direction)
    {
        case TOP :
            invDirection = BOTTOM;
        break;

        case RIGHT :
            invDirection = LEFT;
        break;

        case BOTTOM :
            invDirection = TOP;
        break;

        case LEFT :
            invDirection = RIGHT;
        break;

        default:
            invDirection = BOTTOM;
        break;
    }

    m_neighbours[direction] = nullptr;
    neighbour->m_neighbours[invDirection] = nullptr;
}

void NzTerrainQuadTree::Initialize()
{
    m_isInitialized = true;
    //On subdivise l'arbre équitablement au niveau minimum
    m_root->HierarchicalSubdivide(m_commonConfiguration.minPrecision,true);
    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    m_root->HierarchicalSlopeBasedSubdivide(m_commonConfiguration.maxSlopePrecision);
}

//void NzTerrainQuadTree::Render()
//{
    //m_dispatcher.Draw();
//}

NzTerrainQuadTree* NzTerrainQuadTree::GetContainingQuadTree(const NzTerrainNodeID& nodeID)
{
    if(nodeID.locx < 0)
        return m_neighbours[LEFT];

    if(nodeID.locy < 0)
        return m_neighbours[TOP];

    if(nodeID.locx > (std::pow(2,nodeID.depth)-1))
        return m_neighbours[RIGHT];

    if(nodeID.locy > (std::pow(2,nodeID.depth)-1))
        return m_neighbours[BOTTOM];

    return nullptr;
}

NzTerrainQuadTree* NzTerrainQuadTree::GetNeighbourQuadTree(nzDirection direction)
{
    return m_neighbours[direction];
}

nzDirection NzTerrainQuadTree::GetNeighbourDirection(NzTerrainQuadTree* neighbour)
{
    return m_connectionDirectionLookup[neighbour];
}

nzConnectionType NzTerrainQuadTree::GetConnectionType(NzTerrainQuadTree* neighbour)
{
    std::map<NzTerrainQuadTree*,nzConnectionType>::iterator it = m_connectionType.find(neighbour);

    if(it == m_connectionType.end())
        return nzConnectionType_none;
    else
        return it->second;
}

unsigned int NzTerrainQuadTree::GetLeafNodesAmount() const
{
    return m_leaves.size();
}

float NzTerrainQuadTree::GetMaximumHeight() const
{
    return m_commonConfiguration.maxHeight;
}

NzTerrainInternalNode* NzTerrainQuadTree::GetNode(const NzTerrainNodeID& nodeID)
{
    if(m_nodesMap.count(nodeID) == 1)
        return m_nodesMap.at(nodeID);
    else
        return nullptr;
}

NzTerrainInternalNode* NzTerrainQuadTree::GetRootNode()
{
    return m_root;
}

void NzTerrainQuadTree::DeleteNode(NzTerrainNode* node)
{
    //Avant de supprimer un node, on l'envèle des tasks lists si besoin

    std::map<NzTerrainNodeID,NzTerrainInternalNode*>::iterator it = m_refinementQueue.find(node->GetNodeID());

    if(it != m_refinementQueue.end())
        m_refinementQueue.erase(it);

    //Si la camera se déplace très rapidement, un node peut se retrouver à la fois
    //dans la liste de subdivision et de fusion
    it = m_subdivisionQueue.find(node->GetNodeID());
    if(it != m_subdivisionQueue.end())
        m_subdivisionQueue.erase(it);

    it = m_nodesMap.find(node->GetNodeID());

    if(it != m_nodesMap.end())
        m_nodesMap.erase(it);

    m_nodesPool.ReturnObjectPtr(node);
}

unsigned int NzTerrainQuadTree::GetSubdivisionsAmount()
{
    unsigned int temp = m_subdivisionsAmount;
    m_subdivisionsAmount = 0;
    return temp;
}

NzVector3f NzTerrainQuadTree::GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y)
{
    ///Les terrains sont centrées en 0
    ///Avec le système de node
    ///Leur position "affichée" peut changer à l'exécution
    ///La configuration ne peut donc pas contenir la position du terrain, ce doit être géré par lee système de node
    ///Néanmoins, pour un terrain infini, les quadtree autres que le central doivent avoir un offset
    ///Par conséquent la configuration contient la taille du terrain en floattant
    ///Et un offset (x,y) en coordonnées entières

    //Note : nodeID.depth should never be < 0
    float power = 1.f/(1 << nodeID.depth);
    NzVector3f position;

    switch(m_type)
    {
        case TERRAIN:
            position.x = m_terrainConfiguration.terrainSize * (m_commonConfiguration.x_offset + (x * 0.25f + nodeID.locx) * power);
            position.z = m_terrainConfiguration.terrainSize * (m_commonConfiguration.y_offset + (y * 0.25f + nodeID.locy) * power);
            position.y = m_heightSource2D->GetHeight(position.x,position.z) * m_commonConfiguration.maxHeight;
            return m_rotationMatrix.Transform(position);
        break;

        case PLANET:
        default:

            //Les coordonnées d'un plan
            position.x = 2.f * (x * 0.25f + nodeID.locx) / std::pow(2,nodeID.depth) - 1.f;
            position.y = 1.f;
            position.z = 2.f * (y * 0.25f + nodeID.locy) / std::pow(2,nodeID.depth) - 1.f;

            //On normalise le vecteur pour obtenir une sphère
            position.Normalize();
            position *= m_planetConfiguration.planetRadius;

            //On l'oriente correctement
            position = m_rotationMatrix.Transform(position);

            //Et applique la hauteur en cette position (A Optimiser)
            float height = m_heightSource3D->GetHeight(position.x, position.y, position.z) * m_commonConfiguration.maxHeight;
            position.Normalize();
            position *= m_planetConfiguration.planetRadius + height;

            ///position += m_commonConfiguration.center;

            return position;

        break;
    }

}

void NzTerrainQuadTree::RegisterLeaf(NzTerrainInternalNode* node)
{
    if(m_nodesMap.count(node->GetNodeID()) == 0)
    {
        m_leaves.push_back(node);
        m_nodesMap[node->GetNodeID()] = node;
    }
}

bool NzTerrainQuadTree::UnRegisterLeaf(NzTerrainInternalNode* node)
{
    m_leaves.remove(node);

    return true;
}

bool NzTerrainQuadTree::UnRegisterNode(NzTerrainInternalNode* node)
{
    std::map<NzTerrainNodeID,NzTerrainInternalNode*>::iterator it = m_nodesMap.find(node->GetNodeID());

    if(it != m_nodesMap.end())
    {
        m_nodesMap.erase(it);
        return true;
    }
    else
    {
        NazaraError("NzTerrainQuadTree::UnRegisterLeaf : Trying to remove unexisting node" +
                    NzString::Number(node->GetNodeID().depth) + "|" +
                    NzString::Number(node->GetNodeID().locx) + "|" +
                    NzString::Number(node->GetNodeID().locy));
        return false;
    }
}

void NzTerrainQuadTree::Update(const NzVector3f& cameraPosition)
{

    nzUInt64 maxTime = 10;//ms
    std::map<NzTerrainNodeID,NzTerrainInternalNode*>::iterator it;
    int subdivisionsPerFrame = 0;
    updateClock.Restart();

    ///A chaque frame, on recalcule quels noeuds sont dans le périmètre de la caméra
    m_root->Update(cameraPosition);

    ///On subdivise les nodes
    it = m_subdivisionQueue.begin();
    while(updateClock.GetMilliseconds() < maxTime/2.f)
    {
        if(it == m_subdivisionQueue.end())
            break;

        it->second->Subdivide();

        m_subdivisionQueue.erase(it);
        it = m_subdivisionQueue.begin();
        subdivisionsPerFrame++;
    }
    if(subdivisionsPerFrame > m_maxOperationsPerFrame)
        m_maxOperationsPerFrame = subdivisionsPerFrame;

    m_subdivisionsAmount += subdivisionsPerFrame;


     ///On refine les nodes nécessaires
    it = m_refinementQueue.begin();
    while(updateClock.GetMilliseconds() < maxTime)
    {
        if(it == m_refinementQueue.end())
            break;

        if(!(it->second->IsValid()))
            std::cout<<"problem node not valid"<<std::endl;

        if(it->second == nullptr)
            std::cout<<"problem node == nullptr"<<std::endl;

        if(it->second->HierarchicalRefine())
        {
            m_refinementQueue.erase(it++);
        }
        else
            it++;
    }
}

void NzTerrainQuadTree::AddLeaveToSubdivisionQueue(NzTerrainInternalNode* node)
{
    m_subdivisionQueue[node->GetNodeID()] = node;
}

void NzTerrainQuadTree::AddNodeToRefinementQueue(NzTerrainInternalNode* node)
{
    m_refinementQueue[node->GetNodeID()] = node;
}

void NzTerrainQuadTree::TryRemoveNodeFromRefinementQueue(NzTerrainInternalNode* node)
{
    m_refinementQueue.erase(node->GetNodeID());
}
