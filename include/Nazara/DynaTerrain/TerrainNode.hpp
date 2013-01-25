// Copyright (C) 2012 R�mi B�ges
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

//FIX ME : directement utiliser un NzRect � la place du center/size... M�me occupation m�moire & + de facilit� pour les tests cam

class NAZARA_API NzTerrainNode
{
    public:

        NzTerrainNode();
        ~NzTerrainNode();

        //Lib�re la m�moire � partir du niveau minDepth
            //Si un node a une profondeur inf�rieure � minDepth, elle ne sera pas supprim�e
        void CleanTree(unsigned int minDepth);
        //Subdivise l'ensemble de l'arbre jusqu'� atteindre la profondeur demand�e
            //Il sera par la suite impossible de refiner en dessous de minDepth
        void HierarchicalSubdivide(unsigned int maxDepth);
        //La classe NzPatch est charg�e de communiquer avec le dispatcher (affichage) et de l'utilisation de la source de hauteur
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

        //Subdivise les nodes n�c�ssaires pour obtenir un terrain suffisamment d�fini lors des variations importantes de pente
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
        bool m_isInitialized;//FIX ME : Am�liorer management

        id m_nodeID;
        NzCubef m_aabb;
        NzPatch* m_patch;
        NzVector2f m_center;
        NzVector3f m_realCenter;
        float m_size;
        nzLocation m_location;

        static int nbNodes;

        const int antiInfiniteLoop;//FIX ME : A supprimer ?

        /* Variables pour les fonctionnalit�s suppl�mentaires */

        //Indique que le node ne doit pas �tre refin�, pour conserver une pr�cision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // NAZARA_TERRAINNODE_HPP
