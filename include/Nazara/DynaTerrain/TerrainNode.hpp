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
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>

class NzTerrainQuadTree;
class NzHeightSource;

class NAZARA_API NzTerrainNode
{
    public:

        NzTerrainNode();
        ~NzTerrainNode();

        void CleanTree(unsigned int minDepth);
        void CreatePatch();

        void DebugDrawAABB(bool leafOnly = true, int level = 0);
        void DeletePatch();

        const NzCubef& GetAABB() const;
        NzTerrainNode* GetChild(nzLocation location);
        NzTerrainNode* GetChild(unsigned int i);
        unsigned int GetLevel() const;
        NzTerrainNode* GetDirectNeighbor(nzDirection direction);
        static int GetNodeAmount();
        const id& GetNodeID() const;
        NzTerrainNode* GetParent();

        //void Update(const NzVector3f& cameraPosition, const NzCubef& largerFOV);
        void Update(const NzVector3f& cameraPosition);
        //void HierarchicalAddToCameraList(const NzCubef & cameraFOV, unsigned int maximumDepth);
        //void HierarchicalAddAllChildrenToCameraList(unsigned int maximumDepth);
        bool HierarchicalRefine();
        void HierarchicalSubdivide(unsigned int maxDepth, bool registerAsDynamic = false);
        void HierarchicalSlopeBasedSubdivide(unsigned int maxDepth);

        bool IsLeaf() const;
        bool IsMinimalPrecision() const;
        bool IsRoot() const;
        bool IsRefineable() const;
        bool IsValid() const;
        void Initialize(TerrainNodeData *data, NzTerrainNode* parent, nzLocation loc = TOPLEFT);
        void Invalidate();

        bool Refine();

        bool Subdivide(bool registerAsDynamic = false);

    private:
        void HandleNeighborSubdivision(nzDirection direction, bool registerAsDynamic = false);
        /* Variables pour le fonctionnement basique de l'arbre */
        TerrainNodeData* m_data;
        NzTerrainNode* m_parent;
        NzTerrainNode* m_children[4];

        bool m_isLeaf;
        bool m_isRoot;
        bool m_patchMemoryAllocated;
        bool m_isInitialized;

        id m_nodeID;
        NzCubef m_aabb;
        NzPatch* m_patch;
        nzLocation m_location;

        static int nbNodes;

        /* Variables pour les fonctionnalités supplémentaires */

        //Indique que le node ne doit pas être refiné, pour conserver une précision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // NAZARA_TERRAINNODE_HPP
