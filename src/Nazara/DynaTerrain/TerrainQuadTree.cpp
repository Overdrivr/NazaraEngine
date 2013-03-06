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
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzTerrainQuadTree::NzTerrainQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource)
{
    m_nodesPool.SetChunkSize(5000);
    m_patchesPool.SetChunkSize(1000);

    if(configuration.IsValid())
        m_configuration = configuration;

    m_heightSource = heightSource;
    m_data.quadtree = this;
    m_data.heightSource = m_heightSource;
    m_data.dispatcher = &m_dispatcher;
    m_rotationMatrix.MakeRotation(NzQuaternionf(m_configuration.terrainOrientation));

    m_dispatcher.Initialize(m_configuration.minTerrainPrecision,0);

    m_root = m_nodesPool.GetObjectPtr();
    m_root->Initialize(&m_data,nullptr);
    m_leaves.push_back(m_root);//I
    m_nodesMap[NzTerrainNodeID(0,0,0)] = m_root;

    m_isInitialized = false;

    m_subdivisionsAmount = 0;

    float radius = m_configuration.higherCameraPrecisionRadius;

    for(int i(0) ; i < m_configuration.cameraRadiusAmount ; ++i)
    {
        m_cameraRadiuses[radius] = m_configuration.higherCameraPrecision - i;

        std::cout<<"radius "<<m_configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= m_configuration.radiusSizeIncrement;
    }

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
    cout<<"Maximum amount of operations per frame : "<<m_maxOperationsPerFrame<<std::endl;
    cout<<"Libération de "<<m_nodesPool.GetPoolSize()<<" node(s), veuillez patientez..."<<endl;
    NzClock clk;
    clk.Restart();
    m_nodesPool.ReleaseAll();
    m_patchesPool.ReleaseAll();
    clk.Pause();
    cout<<"Arbre libere en "<<clk.GetMilliseconds()/1000.f<<" s "<<endl;
    cout<<"NbNodes non supprimes : "<<m_root->GetNodeAmount()<< endl;
}

void NzTerrainQuadTree::ConnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection direction)
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

    m_neighbours[direction] = neighbour;
    neighbour->m_neighbours[invDirection] = this;
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

void NzTerrainQuadTree::Render()
{
    m_data.dispatcher->DrawAll();
}

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

unsigned int NzTerrainQuadTree::GetLeafNodesAmount() const
{
    return m_leaves.size();
}

NzTerrainNode* NzTerrainQuadTree::GetNode(const NzTerrainNodeID& nodeID)
{
    if(m_nodesMap.count(nodeID) == 1)
        return m_nodesMap.at(nodeID);
    else
        return nullptr;
}

float NzTerrainQuadTree::GetMaximumHeight() const
{
    return m_configuration.maxTerrainHeight;
}

NzTerrainNode* NzTerrainQuadTree::GetRootNode()
{
    return m_root;
}

NzTerrainNode* NzTerrainQuadTree::GetNodeFromPool()
{
    return m_nodesPool.GetObjectPtr();
}

NzVector3f NzTerrainQuadTree::GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y)
{
    NzVector3f position;
    position.x = m_configuration.terrainSize * (x * 0.25f + nodeID.locx) / std::pow(2,nodeID.depth) - m_configuration.terrainSize/2.f + m_configuration.terrainCenter.x;
    position.z = m_configuration.terrainSize * (y * 0.25f + nodeID.locy) / std::pow(2,nodeID.depth) - m_configuration.terrainSize/2.f + m_configuration.terrainCenter.z;
    position.y = m_heightSource->GetHeight(position.x,position.z) * m_configuration.maxTerrainHeight + m_configuration.terrainCenter.y;
    return m_rotationMatrix.Transform(position);
}

void NzTerrainQuadTree::ReturnNodeToPool(NzTerrainNode* node)
{
    std::map<NzTerrainNodeID,NzTerrainNode*>::iterator it = m_refinementQueue.find(node->GetNodeID());

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

NzPatch* NzTerrainQuadTree::GetPatchFromPool()
{
    return m_patchesPool.GetObjectPtr();
}

void NzTerrainQuadTree::ReturnPatchToPool(NzPatch* patch)
{
    m_patchesPool.ReturnObjectPtr(patch);
}

unsigned int NzTerrainQuadTree::GetSubdivisionsAmount()
{
    unsigned int temp = m_subdivisionsAmount;
    m_subdivisionsAmount = 0;
    return temp;
}

void NzTerrainQuadTree::Initialize()
{
    m_isInitialized = true;

    //On subdivise l'arbre équitablement au niveau minimum
    m_root->HierarchicalSubdivide(m_configuration.minTerrainPrecision,true);

    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    m_root->HierarchicalSlopeBasedSubdivide(m_configuration.maxSlopePrecision);

}

void NzTerrainQuadTree::RegisterLeaf(NzTerrainNode* node)
{
    if(m_nodesMap.count(node->GetNodeID()) == 0)
    {
        m_leaves.push_back(node);
        m_nodesMap[node->GetNodeID()] = node;
    }
}

bool NzTerrainQuadTree::UnRegisterLeaf(NzTerrainNode* node)
{
    m_leaves.remove(node);

    return true;
}

bool NzTerrainQuadTree::UnRegisterNode(NzTerrainNode* node)
{
    std::map<NzTerrainNodeID,NzTerrainNode*>::iterator it = m_nodesMap.find(node->GetNodeID());

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

    nzUInt64 maxTime = 100;//ms
    std::map<NzTerrainNodeID,NzTerrainNode*>::iterator it;
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

void NzTerrainQuadTree::AddLeaveToSubdivisionQueue(NzTerrainNode* node)
{
    m_subdivisionQueue[node->GetNodeID()] = node;
}

void NzTerrainQuadTree::AddNodeToRefinementQueue(NzTerrainNode* node)
{
    m_refinementQueue[node->GetNodeID()] = node;
}

void NzTerrainQuadTree::TryRemoveNodeFromRefinementQueue(NzTerrainNode* node)
{
    m_refinementQueue.erase(node->GetNodeID());
}

int NzTerrainQuadTree::TransformDistanceToCameraInRadiusIndex(float distance)
{
    if(distance > m_cameraRadiuses.rbegin()->first)
        return -1;

    if(distance < m_cameraRadiuses.begin()->first)
        return m_configuration.higherCameraPrecision;

    it = m_cameraRadiuses.lower_bound(distance);

    if(it != m_cameraRadiuses.end())
        return it->second;

    return -2;
}
