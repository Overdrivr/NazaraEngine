// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef QUADCELL_HPP
#define QUADCELL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Circle.hpp>
#include "Patch.hpp"
#include "HeightSource.hpp"
#include "Enumerations.hpp"

class NzTerrainQuadTree;
class NzHeightSource;

//FIX ME : directement utiliser un NzRect à la place du center/size... Même occupation mémoire & + de facilité pour les tests cam

class NzTerrainNode
{
    public:

        NzTerrainNode(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, const NzVector2f& size, nzLocation loc = TOPLEFT);
        ~NzTerrainNode();

        //Libère la mémoire à partir du niveau minDepth
            //Si un node a une profondeur inférieure à minDepth, elle ne sera pas supprimée
        void CleanTree(unsigned int minDepth);
        //Subdivise l'ensemble de l'arbre jusqu'à atteindre la profondeur demandée
            //Il sera par la suite impossible de refiner en dessous de minDepth
        void HierarchicalSubdivide(unsigned int maxDepth);
        //La classe NzPatch est chargée de communiquer avec le dispatcher (affichage) et de l'utilisation de la source de hauteur
        void CreatePatch(const NzVector2f& center, const NzVector2f& size);
        void DeletePatch();

        NzTerrainNode* GetChild(nzLocation location);
        const NzVector2f& GetCenter() const;
        const NzVector2f& GetSize() const;
        unsigned int GetLevel() const;
        static int GetNodeAmount();
        const id& GetNodeID() const;
        NzTerrainNode* GetParent();
        const NzVector3f& GetRealCenter() const;

        bool IsLeaf() const;
        bool IsRoot() const;

        void HierarchicalAddToCameraList(const NzCirclef& cameraRadius, unsigned int indexRadius);
        void HierarchicalAddAllChildrenToCameraList(unsigned int indexRadius);

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

        id m_nodeID;

        NzPatch* m_patch;
        NzVector2f m_center;
        NzVector3f m_realCenter;
        NzVector2f m_size;
        nzLocation m_location;

        static int nbNodes;

        const int antiInfiniteLoop;

        /* Variables pour les fonctionnalités supplémentaires */

        //Indique que le node ne doit pas être refiné, pour conserver une précision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // QUADCELL_HPP
