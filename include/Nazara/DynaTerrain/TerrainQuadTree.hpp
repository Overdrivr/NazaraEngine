// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINQUADTREE_HPP
#define NAZARA_TERRAINQUADTREE_HPP

#include <Nazara/Prerequesites.hpp>

#include <list>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/DynaTerrain/TerrainNode.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTreeConfiguration.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/Renderer/Texture.hpp>

class NAZARA_API NzTerrainQuadTree
{
    public:
        friend class NzTerrainNode;
        NzTerrainQuadTree(const NzTerrainQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource);
        ~NzTerrainQuadTree();

        bool Contains(id nodeId);

        void DebugDrawAABB(bool leafOnly, int level);

        unsigned int GetLeafNodesAmount() const;
        float GetMaximumHeight() const;
        NzTerrainNode* GetNode(id nodeID);
        NzTerrainNode* GetRootNode();
        unsigned int GetSubdivisionsAmount();
        NzVector3f GetVertexPosition(id ID, int x, int y);

        void Initialize(const NzString& vertexShader, const NzString& fragmentShader, const NzString& terrainTilesTexture);

        void Render();

        bool SetShaders(const NzString& vertexShader, const NzString& fragmentShader);

        void Update(const NzVector3f& cameraPosition);

    protected:

        NzTerrainNode* GetNodeFromPool();
        void ReturnNodeToPool(NzTerrainNode* node);
        NzPatch* GetPatchFromPool();
        void ReturnPatchToPool(NzPatch* patch);
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont peut être inutiles, y compris maintenir à jour m_leaves
        //Peut éventuellement servir à optimiser Update(..)
        void RegisterLeaf(NzTerrainNode* node);
        bool UnRegisterLeaf(NzTerrainNode* node);//Les feuilles enlevées ici doivent aussi l'être de la camera list
        bool UnRegisterNode(NzTerrainNode* node);

        void AddLeaveToSubdivisionList(NzTerrainNode* node);
        void AddNodeToRefinementList(NzTerrainNode* node);

        //Returns -1 if the distance to the camera is too big
        //or the radius index otherwise
        int TransformDistanceToCameraInRadiusIndex(float distance);

    private:

        NzShader m_shader;
        NzTerrainNode* m_root;
        TerrainNodeData m_data;
        NzDispatcher m_dispatcher;
        NzHeightSource* m_heightSource;
        NzTexture m_terrainTexture;
        NzTerrainQuadTreeConfiguration m_configuration;

        std::map<float,unsigned int> m_cameraRadiuses;
        std::map<float,unsigned int>::iterator it;

        std::map<id,NzTerrainNode*> m_nodesMap;
        std::list<NzTerrainNode*> m_leaves;

        NzObjectPool<NzTerrainNode> m_nodesPool;
        NzObjectPool<NzPatch> m_patchesPool;

        std::map<id,NzTerrainNode*> m_subdivisionQueue;
        std::map<id,NzTerrainNode*> m_refinementQueue;

        unsigned int m_subdivisionsAmount;
        unsigned int m_poolReallocationSize;
        unsigned int m_poolAllocatedSpace;

        bool m_isInitialized;

        unsigned int m_maxOperationsPerFrame;
};

#endif // NAZARA_TERRAINQUADTREE_HPP
