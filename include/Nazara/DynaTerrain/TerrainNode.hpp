// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef QUADCELL_HPP
#define QUADCELL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include "Patch.hpp"
#include "StackArray2D.hpp"
#include "HeightSource.hpp"
#include "Enumerations.hpp"

class NzTerrainQuadTree;
class NzHeightSource;

class NzTerrainNode
{
    public:

        NzTerrainNode(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, const NzVector2f& size, nzLocation loc = TOPLEFT);
        ~NzTerrainNode();
        //Lib�re la m�moire � partir du niveau minDepth
            //Si un node a une profondeur inf�rieure � minDepth, elle ne sera pas supprim�e
        void CleanTree(unsigned int minDepth);
        //Subdivise l'ensemble de l'arbre jusqu'� atteindre la profondeur demand�e
            //Il sera par la suite impossible de refiner en dessous de minDepth
        void HierarchicalSubdivide(unsigned int maxDepth);
        //La classe NzPatch est charg�e de communiquer avec le dispatcher (affichage) et de l'utilisation de la source de hauteur
        void CreatePatch(const NzVector2f& center, const NzVector2f& size);
        void DeletePatch();

        NzTerrainNode* GetChild(nzLocation location);
        unsigned int GetLevel() const;
        static int GetNodeAmount();
        const id& GetNodeID() const;
        bool TestNodeIDIsOutsideQuadTree(id nodeId);
        NzTerrainNode* GetParent();

        //Renvoie true si le node est a une extr�mit� de l'arbre
        bool IsLeaf() const;
        //Renvoie true si le node est la racine de l'arbre, celui dont tous les autres nodes sont rattach�s
        bool IsRoot() const;
        //Subdivise les nodes n�c�ssaires pour obtenir un terrain suffisamment d�fini lors des variations importantes de pente
        void SlopeBasedHierarchicalSubdivide(unsigned int maxDepth);
        bool Subdivide();
        void Refine();

    private:
        void HandleNeighborSubdivision(nzDirection direction);
        /* Variables pour le fonctionnement basique de l'arbre */
        TerrainNodeData* m_data;
        NzTerrainNode* m_parent;
        NzTerrainNode* m_topLeftLeaf;
        NzTerrainNode* m_topRightLeaf;
        NzTerrainNode* m_bottomLeftLeaf;
        NzTerrainNode* m_bottomRightLeaf;

        bool m_isLeaf;
        bool m_isRoot;
        bool m_patchMemoryAllocated;

        id m_nodeID;

        NzPatch* m_patch;
        NzVector2f m_center;
        NzVector2f m_size;
        nzLocation m_location;

        static int nbNodes;

        const int antiInfiniteLoop;

        /* Variables pour les fonctionnalit�s suppl�mentaires */

        //Indique que le node ne doit pas �tre refin�, pour conserver une pr�cision du terrain lors de variation de pente
        bool m_doNotRefine;
};

#endif // QUADCELL_HPP
