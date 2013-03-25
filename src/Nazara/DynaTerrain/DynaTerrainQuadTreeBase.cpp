// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/DynaTerrain/DynaTerrainQuadTreeBase.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <cmath>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzDynaTerrainQuadTreeBase::NzDynaTerrainQuadTreeBase(const NzTerrainConfiguration& configuration, NzHeightSource2D* heightSource)
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

NzDynaTerrainQuadTreeBase::NzDynaTerrainQuadTreeBase(const NzPlanetConfiguration& configuration, NzHeightSource3D* heightSource, const NzEulerAnglesf& quadtreeOrientation)
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

void NzDynaTerrainQuadTreeBase::Construct()
{
    m_nodesPool.SetChunkSize(5000);
    m_patchesPool.SetChunkSize(1000);


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

    float radius = m_commonConfiguration.higherCameraPrecisionRadius;
    for(int i(0) ; i < m_commonConfiguration.cameraRadiusAmount ; ++i)
    {
        m_cameraRadiuses[radius] = m_commonConfiguration.higherCameraPrecision - i;
        //std::cout<<"radius "<<m_configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= m_commonConfiguration.radiusSizeIncrement;
    }

}

NzDynaTerrainQuadTreeBase::~NzDynaTerrainQuadTreeBase()
{
    cout<<"Maximum amount of operations per frame : "<<m_maxOperationsPerFrame<<std::endl;
    cout<<"Lib�ration de "<<m_nodesPool.GetPoolSize()<<" node(s), veuillez patientez..."<<endl;
    NzClock clk;
    clk.Restart();
    m_nodesPool.ReleaseAll();
    m_patchesPool.ReleaseAll();
    clk.Pause();
    cout<<"Arbre libere en "<<clk.GetMilliseconds()/1000.f<<" s "<<endl;
    cout<<"NbNodes non supprimes : "<<m_root->GetNodeAmount()<< endl;
    m_isInitialized = false;
}

void NzDynaTerrainQuadTreeBase::ConnectNeighbor(NzDynaTerrainQuadTreeBase* neighbour, nzDirection direction)
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

void NzDynaTerrainQuadTreeBase::DisconnectNeighbor(NzDynaTerrainQuadTreeBase* neighbour, nzDirection direction)
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

void NzDynaTerrainQuadTreeBase::Initialize()
{
    m_isInitialized = true;

    //On subdivise l'arbre �quitablement au niveau minimum
    m_root->HierarchicalSubdivide(m_commonConfiguration.minPrecision,true);

    //Si on doit am�liorer l'arbre l� o� la pente est la plus forte, on le fait �galement
    m_root->HierarchicalSlopeBasedSubdivide(m_commonConfiguration.maxSlopePrecision);

}

void NzDynaTerrainQuadTreeBase::Render()
{
    m_dispatcher.Draw();
}

NzDynaTerrainQuadTreeBase* NzDynaTerrainQuadTreeBase::GetContainingQuadTree(const NzTerrainNodeID& nodeID)
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

unsigned int NzDynaTerrainQuadTreeBase::GetLeafNodesAmount() const
{
    return m_leaves.size();
}

float NzDynaTerrainQuadTreeBase::GetMaximumHeight() const
{
    return m_commonConfiguration.maxHeight;
}

NzTerrainInternalNode* NzDynaTerrainQuadTreeBase::GetNode(const NzTerrainNodeID& nodeID)
{
    if(m_nodesMap.count(nodeID) == 1)
        return m_nodesMap.at(nodeID);
    else
        return nullptr;
}

NzTerrainInternalNode* NzDynaTerrainQuadTreeBase::GetRootNode()
{
    return m_root;
}

NzTerrainInternalNode* NzDynaTerrainQuadTreeBase::GetNodeFromPool()
{
    return m_nodesPool.GetObjectPtr();
}

void NzDynaTerrainQuadTreeBase::ReturnNodeToPool(NzTerrainInternalNode* node)
{
    std::map<NzTerrainNodeID,NzTerrainInternalNode*>::iterator it = m_refinementQueue.find(node->GetNodeID());

    if(it != m_refinementQueue.end())
        m_refinementQueue.erase(it);

    //Si la camera se d�place tr�s rapidement, un node peut se retrouver � la fois
    //dans la liste de subdivision et de fusion
    it = m_subdivisionQueue.find(node->GetNodeID());
    if(it != m_subdivisionQueue.end())
        m_subdivisionQueue.erase(it);

    it = m_nodesMap.find(node->GetNodeID());

    if(it != m_nodesMap.end())
        m_nodesMap.erase(it);

    m_nodesPool.ReturnObjectPtr(node);
}

NzPatch* NzDynaTerrainQuadTreeBase::GetPatchFromPool()
{
    return m_patchesPool.GetObjectPtr();
}

void NzDynaTerrainQuadTreeBase::ReturnPatchToPool(NzPatch* patch)
{
    m_patchesPool.ReturnObjectPtr(patch);
}

unsigned int NzDynaTerrainQuadTreeBase::GetSubdivisionsAmount()
{
    unsigned int temp = m_subdivisionsAmount;
    m_subdivisionsAmount = 0;
    return temp;
}

NzVector3f NzDynaTerrainQuadTreeBase::GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y)
{
    //Note : nodeID.depth should never be < 0
    float power = 1.f/(1 << nodeID.depth);
    NzVector3f position;
    switch(m_type)
    {
        case TERRAIN:
            position.x = m_terrainConfiguration.terrainSize * (x * 0.25f + nodeID.locx) * power - m_halfTerrainSize + m_commonConfiguration.center.x;
            position.z = m_terrainConfiguration.terrainSize * (y * 0.25f + nodeID.locy) * power - m_halfTerrainSize + m_commonConfiguration.center.z;
            position.y = m_heightSource2D->GetHeight(position.x,position.z) * m_commonConfiguration.maxHeight + m_commonConfiguration.center.y;
            return m_rotationMatrix.Transform(position);
        break;

        case PLANET:
        default:

            //Les coordonn�es d'un plan
            position.x = 2.f * (x * 0.25f + nodeID.locx) / std::pow(2,nodeID.depth) - 1.f;
            position.y = 1.f;
            position.z = 2.f * (y * 0.25f + nodeID.locy) / std::pow(2,nodeID.depth) - 1.f;

            //On normalise le vecteur pour obtenir une sph�re
            position.Normalize();
            position *= m_planetConfiguration.planetRadius;

            //On l'oriente correctement
            position = m_rotationMatrix.Transform(position);

            //Et applique la hauteur en cette position (A Optimiser)
            float height = m_heightSource3D->GetHeight(position.x, position.y, position.z) * m_commonConfiguration.maxHeight;
            position.Normalize();
            position *= m_planetConfiguration.planetRadius + height;

            position += m_commonConfiguration.center;

            return position;

        break;
    }

}

void NzDynaTerrainQuadTreeBase::RegisterLeaf(NzTerrainInternalNode* node)
{
    if(m_nodesMap.count(node->GetNodeID()) == 0)
    {
        m_leaves.push_back(node);
        m_nodesMap[node->GetNodeID()] = node;
    }
}

bool NzDynaTerrainQuadTreeBase::UnRegisterLeaf(NzTerrainInternalNode* node)
{
    m_leaves.remove(node);

    return true;
}

bool NzDynaTerrainQuadTreeBase::UnRegisterNode(NzTerrainInternalNode* node)
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

void NzDynaTerrainQuadTreeBase::Update(const NzVector3f& cameraPosition)
{

    nzUInt64 maxTime = 5000;//ms
    std::map<NzTerrainNodeID,NzTerrainInternalNode*>::iterator it;
    int subdivisionsPerFrame = 0;
    updateClock.Restart();

    ///A chaque frame, on recalcule quels noeuds sont dans le p�rim�tre de la cam�ra
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


     ///On refine les nodes n�cessaires
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

void NzDynaTerrainQuadTreeBase::AddLeaveToSubdivisionQueue(NzTerrainInternalNode* node)
{
    m_subdivisionQueue[node->GetNodeID()] = node;
}

void NzDynaTerrainQuadTreeBase::AddNodeToRefinementQueue(NzTerrainInternalNode* node)
{
    m_refinementQueue[node->GetNodeID()] = node;
}

void NzDynaTerrainQuadTreeBase::TryRemoveNodeFromRefinementQueue(NzTerrainInternalNode* node)
{
    m_refinementQueue.erase(node->GetNodeID());
}

int NzDynaTerrainQuadTreeBase::TransformDistanceToCameraInRadiusIndex(float distance)
{
    if(distance > m_cameraRadiuses.rbegin()->first)
        return -1;

    if(distance < m_cameraRadiuses.begin()->first)
        return m_commonConfiguration.higherCameraPrecision;

    it = m_cameraRadiuses.lower_bound(distance);

    if(it != m_cameraRadiuses.end())
        return it->second;

    return -2;
}
