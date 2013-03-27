// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAININTERNALNODE_HPP
#define NAZARA_TERRAININTERNALNODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/DynaTerrain/Patch.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>

class NzTerrainQuadTree;
class NzHeightSource;

class NAZARA_API NzTerrainInternalNode
{
    public:

        NzTerrainInternalNode();
        ~NzTerrainInternalNode();

        void CleanTree(unsigned int minDepth);
        void CreatePatch();

        void DeletePatch();

        const NzCubef& GetAABB() const;
        NzTerrainInternalNode* GetChild(nzLocation location);
        NzTerrainInternalNode* GetChild(unsigned int i);
        unsigned int GetLevel() const;
        NzTerrainInternalNode* GetDirectNeighbor(nzDirection direction);
        static int GetNodeAmount();
        const NzTerrainNodeID& GetNodeID() const;
        NzTerrainInternalNode* GetParent();

        void Update(const NzVector3f& cameraPosition);
        bool HierarchicalRefine();
        void HierarchicalSubdivide(unsigned int maxDepth, bool isNotReversible = false);
        void HierarchicalSlopeBasedSubdivide(unsigned int maxDepth);

        bool IsLeaf() const;
        bool IsRoot() const;
        bool IsValid() const;
        void Initialize(TerrainNodeData *data, NzTerrainInternalNode* parent, nzLocation loc = TOPLEFT);
        void Invalidate();

        bool Refine();

        bool Subdivide(bool isNotReversible = false);

    private:
        void HandleNeighborSubdivision(nzDirection direction, bool isNotReversible = false);
        /* Variables pour le fonctionnement basique de l'arbre */
        TerrainNodeData* m_data;
        NzTerrainInternalNode* m_parent;
        NzTerrainInternalNode* m_children[4];

        bool m_isLeaf;
        bool m_isRoot;
        bool m_patchMemoryAllocated;
        bool m_isInitialized;

        NzTerrainNodeID m_nodeID;
        NzCubef m_aabb;
        NzPatch* m_patch;
        nzLocation m_location;

        static int nbNodes;

        /* Variables pour les fonctionnalités supplémentaires */

        //Indique que le node ne doit pas être refiné, pour conserver une précision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // NAZARA_TERRAININTERNALNODE_HPP
