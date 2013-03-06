// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINQUADTREE_HPP
#define NAZARA_TERRAINQUADTREE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/DynaTerrain/TerrainNode.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>

class NzTerrainQuadTree;

class NAZARA_API NzTerrainQuadTree
{
    public:
        friend class NzTerrainNode;

        NzTerrainQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource);
        ~NzTerrainQuadTree();

        void ConnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection direction);

        void DisconnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection direction);

        NzTerrainQuadTree* GetContainingQuadTree(const NzTerrainNodeID& nodeID);
        unsigned int GetLeafNodesAmount() const;
        float GetMaximumHeight() const;
        NzTerrainNode* GetNode(const NzTerrainNodeID& nodeID);
        NzTerrainNode* GetRootNode();
        unsigned int GetSubdivisionsAmount();
        virtual NzVector3f GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y);

        void Initialize();

        void Render();

        void Update(const NzVector3f& cameraPosition);

    protected:

        NzTerrainNode* GetNodeFromPool();
        void ReturnNodeToPool(NzTerrainNode* node);
        NzPatch* GetPatchFromPool();
        void ReturnPatchToPool(NzPatch* patch);
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont peut être inutiles, y compris maintenir à jour m_leaves
        void RegisterLeaf(NzTerrainNode* node);
        bool UnRegisterLeaf(NzTerrainNode* node);//Les feuilles enlevées ici doivent aussi l'être de la camera list
        bool UnRegisterNode(NzTerrainNode* node);

        void AddLeaveToSubdivisionQueue(NzTerrainNode* node);
        void AddNodeToRefinementQueue(NzTerrainNode* node);
        void TryRemoveNodeFromRefinementQueue(NzTerrainNode* node);

        //Returns -1 if the distance to the camera is too big
        //or the radius index otherwise
        int TransformDistanceToCameraInRadiusIndex(float distance);

        NzHeightSource* m_heightSource;
        NzTerrainConfiguration m_configuration;
        NzMatrix4f m_rotationMatrix;

        TerrainNodeData m_data;
        NzDispatcher m_dispatcher;

        NzTerrainNode* m_root;

        NzTerrainQuadTree* m_neighbours[4];

        std::map<float,unsigned int> m_cameraRadiuses;
        std::map<float,unsigned int>::iterator it;

        std::map<NzTerrainNodeID,NzTerrainNode*> m_nodesMap;
        std::list<NzTerrainNode*> m_leaves;

        NzObjectPool<NzTerrainNode> m_nodesPool;
        NzObjectPool<NzPatch> m_patchesPool;

        std::map<NzTerrainNodeID,NzTerrainNode*> m_subdivisionQueue;
        std::map<NzTerrainNodeID,NzTerrainNode*> m_refinementQueue;

        unsigned int m_subdivisionsAmount;
        unsigned int m_poolReallocationSize;
        unsigned int m_poolAllocatedSpace;

        bool m_isInitialized;

        unsigned int m_maxOperationsPerFrame;

        NzClock updateClock;

    private:


};

#endif // NAZARA_TERRAINQUADTREE_HPP
