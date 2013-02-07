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

//FIX ME : directement utiliser un NzRect à la place du center/size... Même occupation mémoire & + de facilité pour les tests cam

class NAZARA_API NzTerrainNode
{
    public:

        NzTerrainNode();
        ~NzTerrainNode();

        void CleanTree(unsigned int minDepth);
        void CreatePatch();

        void DebugDrawAABB(bool leafOnly = true, int level = 0);
        void DeletePatch();

        NzCubef GetAABB() const;
        const NzVector2f& GetCenter() const;
        NzTerrainNode* GetChild(nzLocation location);
        NzTerrainNode* GetChild(unsigned int i);
        unsigned int GetLevel() const;
        NzTerrainNode* GetDirectNeighbor(nzDirection direction);
        static int GetNodeAmount();
        const id& GetNodeID() const;
        NzTerrainNode* GetParent();
        const NzVector3f& GetRealCenter() const;
        float GetSize() const;

        //void HierarchicalAddToCameraList(const NzSpheref& cameraFOV, unsigned int maximumDepth);
        void HierarchicalAddToCameraList(const NzCubef & cameraFOV, unsigned int maximumDepth);
        void HierarchicalAddAllChildrenToCameraList(unsigned int maximumDepth);
        bool HierarchicalRefine();
        void HierarchicalSubdivide(unsigned int maxDepth, bool registerAsDynamic = false);
        void HierarchicalSlopeBasedSubdivide(unsigned int maxDepth);

        bool IsLeaf() const;
        bool IsMinimalPrecision() const;
        bool IsRoot() const;
        bool IsRefineable() const;
        bool IsValid() const;
        void Initialize(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, float size, nzLocation loc = TOPLEFT);
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
        bool m_isInitialized;//FIX ME : Améliorer management

        id m_nodeID;
        NzCubef m_aabb;
        NzPatch* m_patch;
        NzVector2f m_center;
        NzVector3f m_realCenter;
        float m_size;
        nzLocation m_location;

        static int nbNodes;

        const int antiInfiniteLoop;//FIX ME : A supprimer ?

        /* Variables pour les fonctionnalités supplémentaires */

        //Indique que le node ne doit pas être refiné, pour conserver une précision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // NAZARA_TERRAINNODE_HPP
