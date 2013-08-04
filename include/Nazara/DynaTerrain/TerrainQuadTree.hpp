// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNATERRAINQUADTREEBASE_HPP
#define NAZARA_DYNATERRAINQUADTREEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/DynaTerrain/TerrainNode.hpp>
//#include <Nazara/DynaTerrain/TerrainMasterNode.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/PlanetConfiguration.hpp>
#include <Nazara/DynaTerrain/HeightSource2D.hpp>
#include <Nazara/DynaTerrain/HeightSource3D.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/Core/Clock.hpp>

//TODO : Gain réels du pool ? voir boost Pool

class NAZARA_API NzTerrainQuadTree
{
    public:
        friend class NzTerrainNode;
        friend class NzTerrainConfiguration;

        NzTerrainQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource2D* heightSource);
        NzTerrainQuadTree(const NzPlanetConfiguration& configuration, NzHeightSource3D* heightSource, const NzEulerAnglesf& quadtreeOrientation = NzEulerAnglesf(0.f,0.f,0.f));
        ~NzTerrainQuadTree();

        // Interaction avec les quadtrees voisins
        void ConnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection callerDirection, nzDirection calleeDirection);
        void DisconnectNeighbor(NzTerrainQuadTree* neighbour, nzDirection direction);//DO NOT USE : Modifs todo
        //NzTerrainQuadTree* GetContainingQuadTree(const NzTerrainNodeID& nodeID);//DO NOT USE : Outdated
        NzTerrainQuadTree* GetNeighbourQuadTree(nzDirection direction);
        nzDirection GetNeighbourDirection(NzTerrainQuadTree* neighbour);
        nzConnectionType GetConnectionType(NzTerrainQuadTree* neighbour);

        // Interaction avec les nodes
        unsigned int GetLeafNodesAmount() const;
        NzTerrainInternalNode* GetNode(const NzTerrainNodeID& nodeID);
        NzTerrainInternalNode* GetRootNode();

        // Informations
        virtual NzVector3f GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y);
        float GetMaximumHeight() const;//??
        unsigned int GetSubdivisionsAmount();//??

        // Opérations principales
        void Initialize();
        //void Render();
        void Update(const NzVector3f& cameraPosition);

    protected:

        nzQuadTreeType m_type;
        NzTerrainNode* m_root;
        nzTerrainNodeData m_data;// A intégrer dans le node ?
        // Pour récupérer un pointer de node à partir de son identifiant
        std::map<NzTerrainNodeID,NzTerrainInternalNode*> m_nodesMap;
        std::list<NzTerrainInternalNode*> m_leaves;

        void DeleteNode(NzTerrainNode* node);

        void RegisterLeaf(NzTerrainInternalNode* node);
        bool UnRegisterLeaf(NzTerrainInternalNode* node);
        bool UnRegisterNode(NzTerrainInternalNode* node);

        // Opérations sur les nodes
        void AddLeaveToSubdivisionQueue(NzTerrainInternalNode* node);
        void AddNodeToRefinementQueue(NzTerrainInternalNode* node);
        void TryRemoveNodeFromRefinementQueue(NzTerrainInternalNode* node);
        std::map<NzTerrainNodeID,NzTerrainInternalNode*> m_subdivisionQueue;
        std::map<NzTerrainNodeID,NzTerrainInternalNode*> m_refinementQueue;

        //NzTerrainMasterNode m_dispatcher;

        // Configuration du terrain, bof bof
        float m_halfTerrainSize;//??
        NzTerrainConfiguration m_terrainConfiguration;
        NzPlanetConfiguration m_planetConfiguration;
        NzDynaTerrainConfigurationBase m_commonConfiguration;

        // Les sources de hauteur, aussi bof bof les 2 en même temps
        NzHeightSource2D* m_heightSource2D;
        NzHeightSource3D* m_heightSource3D;


        // Une matrice outil, utile ?
        NzMatrix4f m_rotationMatrix;

        // Les voisins du quadtree
        NzTerrainQuadTree* m_neighbours[4];
        std::map<NzTerrainQuadTree*,nzConnectionType> m_connectionType;
        std::map<NzTerrainQuadTree*,nzDirection> m_connectionDirectionLookup;

        // Utile ?
        std::map<float,unsigned int>::iterator it;


        // Les pools d'objets, utiles ?


        unsigned int m_subdivisionsAmount;
        unsigned int m_maxOperationsPerFrame;
        NzClock updateClock;

        unsigned int m_poolReallocationSize;
        unsigned int m_poolAllocatedSpace;

        bool m_isInitialized;

    private:
        void Construct();

};

#endif // NAZARA_TERRAINQUADTREE_HPP
