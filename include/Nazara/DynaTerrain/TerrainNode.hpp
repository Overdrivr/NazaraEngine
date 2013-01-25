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

        //Libère la mémoire à partir du niveau minDepth
            //Si un node a une profondeur inférieure à minDepth, elle ne sera pas supprimée
        void CleanTree(unsigned int minDepth);
        //Subdivise l'ensemble de l'arbre jusqu'à atteindre la profondeur demandée
            //Il sera par la suite impossible de refiner en dessous de minDepth
        void HierarchicalSubdivide(unsigned int maxDepth);
        //La classe NzPatch est chargée de communiquer avec le dispatcher (affichage) et de l'utilisation de la source de hauteur
        void CreatePatch();
        void DeletePatch();

        NzCubef GetAABB() const;
        NzTerrainNode* GetChild(nzLocation location);
        const NzVector2f& GetCenter() const;
        float GetSize() const;
        unsigned int GetLevel() const;
        static int GetNodeAmount();
        const id& GetNodeID() const;
        NzTerrainNode* GetParent();
        const NzVector3f& GetRealCenter() const;

        bool IsLeaf() const;
        bool IsRoot() const;
        void Initialize(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, float size, nzLocation loc = TOPLEFT);
        void Invalidate();

        //void HierarchicalAddToCameraList(const NzSpheref& cameraFOV, unsigned int maximumDepth);
        void HierarchicalAddToCameraList(const NzCubef & cameraFOV, unsigned int maximumDepth);
        void HierarchicalAddAllChildrenToCameraList(unsigned int maximumDepth);

        void Refine();//FIX ME : Won't work with camera subdivision because of m_doNotRefine and HierarchicalSubdiv()

        //Subdivise les nodes nécéssaires pour obtenir un terrain suffisamment défini lors des variations importantes de pente
        void SlopeBasedHierarchicalSubdivide(unsigned int maxDepth);
        bool Subdivide();

        bool TestNodeIDIsOutsideQuadTree(id nodeId);

    private:
        void HandleNeighborSubdivision(nzDirection direction);
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
