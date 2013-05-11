// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNATERRAINQUADTREEBASE_HPP
#define NAZARA_DYNATERRAINQUADTREEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/DynaTerrain/TerrainInternalNode.hpp>
#include <Nazara/DynaTerrain/TerrainMasterNode.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/PlanetConfiguration.hpp>
#include <Nazara/DynaTerrain/HeightSource2D.hpp>
#include <Nazara/DynaTerrain/HeightSource3D.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/Core/Clock.hpp>

class NAZARA_API NzTerrainQuadTree
{
    public:
        friend class NzTerrainInternalNode;
        friend class NzTerrainConfiguration;

        NzTerrainQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource2D* heightSource);
        NzTerrainQuadTree(const NzPlanetConfiguration& configuration, NzHeightSource3D* heightSource, const NzEulerAnglesf& quadtreeOrientation = NzEulerAnglesf(0.f,0.f,0.f));
        ~NzTerrainQuadTree();

        void ConnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection callerDirection, nzDirection calleeDirection);

        void DisconnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection direction);//DO NOT USE : Modifs todo

        NzTerrainQuadTree* GetContainingQuadTree(const NzTerrainNodeID& nodeID);//DO NOT USE : Outdated
        NzTerrainQuadTree* GetNeighbourQuadTree(nzDirection direction);
        bool GetIsConnectionStraight(NzTerrainQuadTree* neighbour);
        unsigned int GetLeafNodesAmount() const;
        float GetMaximumHeight() const;
        NzTerrainInternalNode* GetNode(const NzTerrainNodeID& nodeID);
        NzTerrainInternalNode* GetRootNode();
        unsigned int GetSubdivisionsAmount();
        virtual NzVector3f GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y);

        void Initialize();

        void Render();

        void Update(const NzVector3f& cameraPosition);

    protected:
        nzQuadTreeType m_type;
        NzTerrainInternalNode* GetNodeFromPool();
        void ReturnNodeToPool(NzTerrainInternalNode* node);
        NzPatch* GetPatchFromPool();
        void ReturnPatchToPool(NzPatch* patch);
        NzTerrainVertex* GetVertexFromPool();
        void ReturnVertexToPool(NzTerrainVertex* vertex);
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont peut être inutiles, y compris maintenir à jour m_leaves
        void RegisterLeaf(NzTerrainInternalNode* node);
        bool UnRegisterLeaf(NzTerrainInternalNode* node);//Les feuilles enlevées ici doivent aussi l'être de la camera list
        bool UnRegisterNode(NzTerrainInternalNode* node);

        void AddLeaveToSubdivisionQueue(NzTerrainInternalNode* node);
        void AddNodeToRefinementQueue(NzTerrainInternalNode* node);
        void TryRemoveNodeFromRefinementQueue(NzTerrainInternalNode* node);

        //Returns -1 if the distance to the camera is too big
        //or the radius index otherwise
        virtual int TransformDistanceToCameraInRadiusIndex(float distance);

        NzTerrainMasterNode m_dispatcher;

        NzTerrainConfiguration m_terrainConfiguration;
        float m_halfTerrainSize;
        NzPlanetConfiguration m_planetConfiguration;
        NzDynaTerrainConfigurationBase m_commonConfiguration;
        //-------
        NzHeightSource2D* m_heightSource2D;
        NzHeightSource3D* m_heightSource3D;

        NzTerrainInternalNode* m_root;
        TerrainNodeData m_data;

        NzMatrix4f m_rotationMatrix;
        //-------

        NzTerrainQuadTree* m_neighbours[4];
        std::map<NzTerrainQuadTree*,bool> m_connectionType;

        std::map<float,unsigned int> m_cameraRadiuses;
        std::map<float,unsigned int>::iterator it;

        std::map<NzTerrainNodeID,NzTerrainInternalNode*> m_nodesMap;
        std::list<NzTerrainInternalNode*> m_leaves;

        NzObjectPool<NzTerrainInternalNode> m_nodesPool;
        NzObjectPool<NzPatch> m_patchesPool;
        NzObjectPool<NzTerrainVertex> m_verticesPool;

        std::map<NzTerrainNodeID,NzTerrainInternalNode*> m_subdivisionQueue;
        std::map<NzTerrainNodeID,NzTerrainInternalNode*> m_refinementQueue;

        unsigned int m_subdivisionsAmount;
        unsigned int m_poolReallocationSize;
        unsigned int m_poolAllocatedSpace;

        unsigned int m_maxOperationsPerFrame;

        NzClock updateClock;

        bool m_isInitialized;

    private:
        void Construct();

};

#endif // NAZARA_TERRAINQUADTREE_HPP
