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

    //m_buffersAmount = 1+(600*(m_configuration.ComputeMaxPatchNumber()))/1048576;
    //cout<<"1 Mio buffers amount : "<<m_buffersAmount<<endl;

    m_dispatcher.Initialize(m_configuration.minTerrainPrecision,0);

    m_root = m_nodesPool.GetObjectPtr();
    m_root->Initialize(&m_data,nullptr);
    m_leaves.push_back(m_root);//I
    m_nodesMap[id(0,0,0)] = m_root;

    m_isInitialized = false;

    m_currentCameraRadiusIndex = 0;

    m_subdivisionsAmount = 0;

/*
    cameraRadiusStep = m_configuration.effectRadius/(m_configuration.closeCameraDepth - m_configuration.farCameraDepth);
    std::cout<<"pas = "<<cameraRadiusStep<<std::endl;
    for(int i(0) ; i < m_configuration.closeCameraDepth - m_configuration.farCameraDepth ; ++i)
    {

        int index = m_configuration.farCameraDepth - 1 + i;
        float radius = m_configuration.startRadius + m_configuration.effectRadius - i * cameraRadiusStep;
        std::cout<<index<<" : "<<radius<<std::endl;

        NzSpheref cameraFOV(NzVector3f(0.f,0.f,0.f),radius);
        NzSpheref cameraFOVLarge(NzVector3f(0.f,0.f,0.f),radius * 1.5);
        cameraFOVSubdivision.push_back(cameraFOV.GetBoundingCube());
        cameraFOVRefine.push_back(cameraFOVLarge.GetBoundingCube());
    }*/

    std::cout<<"inc = "<<m_configuration.radiusSizeIncrement<<std::endl;
    m_lambda = std::log(1.f / m_configuration.radiusSizeIncrement);
    std::cout<<"lambda = "<<m_lambda<<std::endl;
    float radius = m_configuration.higherCameraPrecisionRadius;

    for(int i(0) ; i < m_configuration.cameraRadiusAmount ; ++i)
    {
        m_cameraRadiuses.push_back(radius);
        std::cout<<"radius "<<i<<" = "<<radius<<std::endl;
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
    return nodeId.lvl < 0 || nodeId.sx < 0 || nodeId.sy < 0 || nodeId.sx > (std::pow(2,nodeId.lvl)-1) || nodeId.sy > (std::pow(2,nodeId.lvl)-1);
}

void NzTerrainQuadTree::Render()
{
    // On active le shader
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
    std::map<id,NzTerrainNode*>::iterator it = m_removeList.find(node->GetNodeID());

    if(it != m_removeList.end())
        m_removeList.erase(it);

    //Si la camera se déplace très rapidement, un node peut se retrouver à la fois
    //dans la liste de subdivision et de fusion
    it = m_subdivideList.find(node->GetNodeID());
    if(it != m_subdivideList.end())
        m_subdivideList.erase(it);

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
    m_root->HierarchicalSubdivide(m_configuration.minTerrainPrecision);

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
    //On supprime un node de la liste active
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
    float radius = 300.f;

    //Une optimisation potentielle à mettre en oeuvre : Au lieu de tester l'ensemble de l'arbre contre le périmètre caméra
    //On teste d'abord l'ensemble de l'arbre sur le périmètre le plus grand
    //On teste ensuite les nodes retenus sur le périmètre suivant (et donc plus petit) et ainsi de suite,
    //On a donc de moins en moins de nodes à tester à chaque fois, mais alors plus d'aspect hiérarchique
    //Les nodes feuilles seront tous testés dés le second périmètre.
    //Cette optimisation sera efficace si traverser l'arbre est plus lent que tester un node contre un périmètre
    //Ce qui est peut probable, mais à tester quand même


    NzClock clock;
    nzUInt64 maxTime = 100;//ms
    clock.Restart();

/*

    cameraFOVSubdivision[0].x = cameraPosition.x - cameraFOVSubdivision[0].width/2.f;
    cameraFOVSubdivision[0].y = cameraPosition.y - cameraFOVSubdivision[0].height/2.f;
    cameraFOVSubdivision[0].z = cameraPosition.z - cameraFOVSubdivision[0].depth/2.f;

    cameraFOVRefine[0].x = cameraPosition.x - cameraFOVRefine[0].width/2.f;
    cameraFOVRefine[0].y = cameraPosition.y - cameraFOVRefine[0].height/2.f;
    cameraFOVRefine[0].z = cameraPosition.z - cameraFOVRefine[0].depth/2.f;*/

    //A chaque frame, on recalcule quels noeuds sont dans le périmètre de la caméra
    m_root->Update(cameraPosition);

    std::map<id,NzTerrainNode*>::iterator it;
    int subdivisionsPerFrame = 0;

    ///On subdivise les nodes
    it = m_subdivideList.begin();

    while(clock.GetMilliseconds() < maxTime/2.f)
    {
        if(it == m_subdivideList.end())
            break;

        it->second->Subdivide(true);

        m_subdivideList.erase(it);
        it = m_subdivideList.begin();
        subdivisionsPerFrame++;
    }

    if(subdivisionsPerFrame > m_maxOperationsPerFrame)
        m_maxOperationsPerFrame = subdivisionsPerFrame;

    m_subdivisionsAmount += subdivisionsPerFrame;


     ///On refine les nodes nécessaires
    it = m_removeList.begin();
    while(clock.GetMilliseconds() < maxTime)
    {
        if(it == m_removeList.end())
            break;

        if(!(it->second->IsValid()))
            std::cout<<"problem node not valid"<<std::endl;

        if(it->second == nullptr)
            std::cout<<"problem node == nullptr"<<std::endl;

        if(it->second->HierarchicalRefine())
        {
            m_removeList.erase(it++);
        }
        else
            it++;
    }

    //std::cout<<"remove queue size : "<<m_removeList.size()<<std::endl;

    ///On refine les nodes nécessaires
    /*it = m_removeList.begin();
    //int cmpt = 5;
    while(clock.GetMilliseconds() < maxTime)
    {
        if(it == m_removeList.end())
            break;

        if(!(it->second->IsValid()))
            std::cout<<"problem node not valid"<<std::endl;

        if(it->second == nullptr)
            std::cout<<"problem node == nullptr"<<std::endl;

        if(!cameraFOVRefine[0].Contains(it->second->GetAABB()) && !cameraFOVRefine[0].Intersect(it->second->GetAABB()))
        {
            if(it->second->HierarchicalRefine())
            {
                m_removeList.erase(it++);
                //cmpt--;
            }
            else
                it++;
        }
        else
           it++;

    }*/
}

void NzTerrainQuadTree::AddLeaveToSubdivisionList(NzTerrainNode* node)
{
    m_subdivideList[node->GetNodeID()] = node;
}

void NzTerrainQuadTree::AddNodeToRefinementList(NzTerrainNode* node)
{
    m_removeList[node->GetNodeID()] = node;
}

int NzTerrainQuadTree::TransformDistanceToCameraInRadiusIndex(float distance)
{
    if(distance > m_cameraRadiuses.back())
        return -1;

    if(distance < m_cameraRadiuses.front())
        return m_configuration.higherCameraPrecision;

    return static_cast<int>(m_configuration.higherCameraPrecision * std::exp(m_lambda * distance / m_configuration.higherCameraPrecisionRadius));
}
