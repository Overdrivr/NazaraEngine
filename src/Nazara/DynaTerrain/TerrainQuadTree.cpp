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

    m_buffersAmount = 1+(600*(m_configuration.ComputeMaxPatchNumber()))/1048576;
    cout<<"1 Mio buffers amount : "<<m_buffersAmount<<endl;

    m_dispatcher.Initialize(m_configuration.minimumDepth,m_buffersAmount);

    m_root = m_nodesPool.GetObjectPtr();
    m_root->Initialize(&m_data,nullptr,terrainCenter,m_configuration.terrainSize);
    m_leaves.push_back(m_root);
    m_nodesMap[id(0,0,0)] = m_root;

    m_isInitialized = false;

    m_currentCameraRadiusIndex = 0;

    m_subdivisionsAmount = 0;

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
    return m_configuration.terrainHeight;
}

NzTerrainNode* NzTerrainQuadTree::GetRootNode()
{
    return m_root;
}

NzTerrainNode* NzTerrainQuadTree::GetNodeFromPool()
{
    return m_nodesPool.GetObjectPtr();
}

void NzTerrainQuadTree::ReturnNodeToPool(NzTerrainNode* node)
{
    std::map<id,NzTerrainNode*>::iterator it = m_removeList.find(node->GetNodeID());

    if(it != m_removeList.end())
        m_removeList.erase(it);

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
    m_root->HierarchicalSubdivide(m_configuration.minimumDepth);

    //Si on doit améliorer l'arbre là où la pente est la plus forte, on le fait également
    m_root->HierarchicalSlopeBasedSubdivide(m_configuration.slopeMaxDepth);

}

void NzTerrainQuadTree::RegisterLeaf(NzTerrainNode* node)
{
    if(m_nodesMap.count(node->GetNodeID()) == 0)
    {
        m_leaves.push_back(node);
        m_nodesMap[node->GetNodeID()] = node;
    }
}

///creation du shader
bool NzTerrainQuadTree::SetShaders(const NzString& vertexShader, const NzString& fragmentShader)
{
    if (!m_shader.Create(nzShaderLanguage_GLSL))
    {
        std::cout << "Failed to load shader" << std::endl;
        std::getchar();
        return false;
    }

    // Le fragment shader traite la couleur de nos pixels
    if (!m_shader.LoadFromFile(nzShaderType_Fragment, fragmentShader))
    {
        std::cout << "Failed to load fragment shader from file" << std::endl;
        // À la différence des autres ressources, le shader possède un log qui peut indiquer les erreurs en cas d'échec
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    // Le vertex shader (Transformation des vertices de l'espace 3D vers l'espace écran)
    if (!m_shader.LoadFromFile(nzShaderType_Vertex, vertexShader))
    {
        std::cout << "Failed to load vertex shader from file" << std::endl;
        std::cout << "Log: " << m_shader.GetLog() << std::endl;
        std::getchar();
        return false;
    }

    // Une fois les codes sources de notre shader chargé, nous pouvons le compiler, afin de le rendre utilisable
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

    //Enlever de la liste de caméra un node si il y est déjà ??
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
    float radius = 100.f;

    //Une optimisation potentielle à mettre en oeuvre : Au lieu de tester l'ensemble de l'arbre contre le périmètre caméra
    //On teste d'abord l'ensemble de l'arbre sur le périmètre le plus grand
    //On teste ensuite les nodes retenus sur le périmètre suivant (et donc plus petit) et ainsi de suite,
    //On a donc de moins en moins de nodes à tester à chaque fois, mais alors plus d'aspect hiérarchique
    //Les nodes feuilles seront tous testés dés le second périmètre.
    //Cette optimisation sera efficace si traverser l'arbre est plus lent que tester un node contre un périmètre
    //Ce qui est peut probable, mais à tester quand même

    NzSpheref cameraFOV(cameraPosition,radius);
    NzSpheref cameraFOVLarge(cameraPosition,radius*1.7);

    //m_subdivideList.clear();
    //m_removeList.clear();

    //A chaque frame, on recalcule quels noeuds sont dans le périmètre de la caméra
    m_root->HierarchicalAddToCameraList(cameraFOV.GetBoundingCube(),3);

    /*if(!m_subdivideList.empty())
        std::cout<<"Subdivisions amount : "<<m_subdivideList.size()<<std::endl;*/
    //if(!m_removeList.empty())
        //std::cout<<"In Subdivided Nodes List : "<<m_removeList.size()<<std::endl;

    //On subdivise les nodes nécessaires
    std::map<id,NzTerrainNode*>::iterator it;

    int subdivisionsPerFrame = 0;

    ///En limitant le nombre de subdivisions par frame
    /*
    unsigned int maxSubdivisionsAmountPerFrame = 1;

    while(subdivisionsPerFrame < maxSubdivisionsAmountPerFrame)
    {
        if(it == m_subdivideList.end())
            break;

        it->second->Subdivide();
        m_subdivideList.erase(it);
        it = m_subdivideList.begin();
        subdivisionsPerFrame++;
    }
    */

    ///On refine les nodes nécessaires
    it = m_removeList.begin();
    int cmpt = 2;
    while(it != m_removeList.end() && cmpt > 0)
    {
        if(!(it->second->IsValid()))
            std::cout<<"problem node not valid"<<std::endl;

        if(it->second == nullptr)
            std::cout<<"problem node == nullptr"<<std::endl;

        if(!cameraFOVLarge.GetBoundingCube().Contains(it->second->GetAABB()) && !cameraFOVLarge.GetBoundingCube().Intersect(it->second->GetAABB()))
        //if(!cameraFOV.Contains(it->second->GetAABB().GetCenter()))
        {
            //std::cout<<"Starting refine"<<std::endl;
            if(it->second->HierarchicalRefine())
            {
                m_removeList.erase(it++);
                //std::cout<<"refine : "<<m_removeList.size()<<std::endl;
            }
            cmpt--;
        }
        else
           it++;

    }

    ///En limitant le temps accordé au terrain par frame
    ///Pas mieux, car la précisions des horloges est limitée, ne pas descendre en dessous d'1 ms
    ///Permet d'estimer grossièrement les performances néanmoins
    NzClock clock;
    nzUInt64 maxTime = 20;//ms
    clock.Restart();

    it = m_subdivideList.begin();

    while(clock.GetMilliseconds() < maxTime)
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

void NzTerrainQuadTree::AddNodeToDynamicList(NzTerrainNode* node)
{
    m_removeList[node->GetNodeID()] = node;
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
