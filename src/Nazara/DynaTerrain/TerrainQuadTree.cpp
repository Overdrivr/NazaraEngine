// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <cmath>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzTerrainQuadTree::NzTerrainQuadTree(const NzTerrainQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource)
{
    m_nodesPool.SetChunkSize(5000);
    m_patchesPool.SetChunkSize(1000);

    if(configuration.IsValid())
        m_configuration = configuration;

    m_heightSource = heightSource;
    m_data.quadtree = this;
    m_data.heightSource = m_heightSource;
    m_data.dispatcher = &m_dispatcher;

    m_dispatcher.Initialize(m_configuration.minTerrainPrecision,0);

    m_root = m_nodesPool.GetObjectPtr();
    m_root->Initialize(&m_data,nullptr);
    m_leaves.push_back(m_root);//I
    m_nodesMap[id(0,0,0)] = m_root;

    m_isInitialized = false;

    m_subdivisionsAmount = 0;


    float radius = m_configuration.higherCameraPrecisionRadius;

    for(int i(0) ; i < m_configuration.cameraRadiusAmount ; ++i)
    {
        m_cameraRadiuses[radius] = m_configuration.higherCameraPrecision - i;

        std::cout<<"radius "<<m_configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= m_configuration.radiusSizeIncrement;
    }

//??
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

bool NzTerrainQuadTree::Contains(id nodeId)
{
    //FIX ME : Wrong ?
    return nodeId.lvl < 0 || nodeId.sx < 0 || nodeId.sy < 0 || nodeId.sx > (std::pow(2,nodeId.lvl)-1) || nodeId.sy > (std::pow(2,nodeId.lvl)-1);
}

void NzTerrainQuadTree::Render()
{
    NzRenderer::SetShader(&m_shader);
    m_data.dispatcher->DrawAll();
}

void NzTerrainQuadTree::DebugDrawAABB(bool leafOnly, int level)
{
    m_root->DebugDrawAABB(leafOnly,level);
}

unsigned int NzTerrainQuadTree::GetLeafNodesAmount() const
{
    return m_leaves.size();
}

NzTerrainNode* NzTerrainQuadTree::GetNode(id nodeID)
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

NzVector3f NzTerrainQuadTree::GetVertexPosition(id ID, int x, int y)
{
    NzVector3f position;
    position.x = m_configuration.terrainSize * (x * 0.25f + ID.sx) / std::pow(2,ID.lvl) - m_configuration.terrainSize/2.f;//FIX ME : Replace pow by << ?
    position.z = m_configuration.terrainSize * (y * 0.25f + ID.sy) / std::pow(2,ID.lvl) - m_configuration.terrainSize/2.f;
    position.y = m_heightSource->GetHeight(position.x,position.z) * m_configuration.maxTerrainHeight;
    return position;
}

void NzTerrainQuadTree::ReturnNodeToPool(NzTerrainNode* node)
{
    std::map<id,NzTerrainNode*>::iterator it = m_refinementQueue.find(node->GetNodeID());

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

void NzTerrainQuadTree::Initialize(const NzString& vertexShader, const NzString& fragmentShader, const NzString& terrainTilesTexture)
{
     m_isInitialized = true;

    SetShaders(vertexShader,fragmentShader);

    if(!m_terrainTexture.LoadFromFile(terrainTilesTexture))
        std::cout<<"Could not load texture "<<terrainTilesTexture<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);

    int i = m_shader.GetUniformLocation("terrainTexture");

    if(i == -1)
        std::cout<<"Could not retrieve uniform location"<<std::endl;

    m_shader.SendTexture(i,&m_terrainTexture);

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

bool NzTerrainQuadTree::SetShaders(const NzString& vertexShader, const NzString& fragmentShader)
{
    if (!m_shader.Create(nzShaderLanguage_GLSL))
    {
        std::cout << "Failed to load shader" << std::endl;
        std::getchar();
        return false;
    }

    if (!m_shader.LoadFromFile(nzShaderType_Fragment, fragmentShader))
    {
        std::cout << "Failed to load fragment shader from file" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    if (!m_shader.LoadFromFile(nzShaderType_Vertex, vertexShader))
    {
        std::cout << "Failed to load vertex shader from file" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    if (!m_shader.Compile())
    {
        std::cout << "Failed to compile shader" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    return true;
}

bool NzTerrainQuadTree::UnRegisterLeaf(NzTerrainNode* node)
{
    m_leaves.remove(node);

    return true;
}

bool NzTerrainQuadTree::UnRegisterNode(NzTerrainNode* node)
{
    std::map<id,NzTerrainNode*>::iterator it = m_nodesMap.find(node->GetNodeID());

    if(it != m_nodesMap.end())
    {
        m_nodesMap.erase(it);
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
    NzClock clock;
    nzUInt64 maxTime = 100;//ms
    clock.Restart();

    //A chaque frame, on recalcule quels noeuds sont dans le périmètre de la caméra
    m_root->Update(cameraPosition);

    std::map<id,NzTerrainNode*>::iterator it;
    int subdivisionsPerFrame = 0;

    ///On subdivise les nodes
    it = m_subdivisionQueue.begin();

    while(clock.GetMilliseconds() < maxTime/2.f)
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
    while(clock.GetMilliseconds() < maxTime)
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

    //std::cout<<"remove queue size : "<<m_removeList.size()<<std::endl;
}

void NzTerrainQuadTree::AddLeaveToSubdivisionList(NzTerrainNode* node)
{
    m_subdivisionQueue[node->GetNodeID()] = node;
}

void NzTerrainQuadTree::AddNodeToRefinementList(NzTerrainNode* node)
{
    m_refinementQueue[node->GetNodeID()] = node;
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
