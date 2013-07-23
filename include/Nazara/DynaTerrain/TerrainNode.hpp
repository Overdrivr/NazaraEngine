// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINNODE_HPP
#define NAZARA_TERRAINNODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/DynaTerrain/Patch.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>

class NzTerrainQuadTree;
class NzHeightSource;

class NAZARA_API NzTerrainNode
{
    public:

        NzTerrainNode();
        ~NzTerrainNode();

        // Actions hiérarchiques
        void CleanTree(unsigned int minDepth);//RENOMMER ReleaseAllChildren
        bool HierarchicalRefine();
        void HierarchicalSubdivide(unsigned int maxDepth, bool isNotReversible = false);
        void HierarchicalSlopeBasedSubdivide(unsigned int maxDepth);

        // Gestion du patch
        void CreatePatch();
        void DeletePatch();

        // Getters
        const NzCubef& GetAABB() const;
        NzTerrainNode* GetChild(nzLocation location);
        NzTerrainNode* GetChild(unsigned int i);
        unsigned int GetLevel() const;
        static int GetNodeAmount();
        const NzTerrainNodeID& GetNodeID() const;
        NzTerrainNode* GetParent();

        // Interaction avec les nodes voisins
        NzTerrainNode* GetDirectNeighbor(nzDirection direction);

        // Actions principales
        void Update(const NzVector3f& cameraPosition);
        void Initialize(TerrainNodeData *data, NzTerrainNode* parent, nzLocation loc = TOPLEFT);
        void Invalidate();
        bool Refine();
        bool Subdivide(bool isNotReversible = false);

        // Infos
        bool IsLeaf() const;
        bool IsRoot() const;
        bool IsValid() const;


    private:
        //?? différence entre les deux
        void Initialize(TerrainNodeData *data, NzTerrainInternalNode* parent, const NzPatch& patch, nzLocation loc = TOPLEFT);
        void InitializeData(TerrainNodeData *data, NzTerrainInternalNode* parent, nzLocation loc = TOPLEFT);

        void HandleNeighborSubdivision(nzDirection direction, bool isNotReversible = false);


        TerrainNodeData* m_data;
        NzTerrainInternalNode* m_parent;
        NzTerrainInternalNode* m_children[4];

        bool m_isLeaf;
        bool m_isRoot;
        bool m_isInitialized;

        //L'identifiant unique du node
        NzTerrainNodeID m_nodeID;
        NzCubef m_aabb;
        NzPatch* m_patch;
        //L'emplacement du node par rapport au parent
        nzLocation m_location;

        static int nbNodes;

        //Indique que le node ne doit pas être refiné, pour conserver une précision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // NAZARA_TERRAINNODE_HPP
