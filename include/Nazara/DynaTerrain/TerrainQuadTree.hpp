// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <Nazara/Prerequesites.hpp>

#include <list>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include "TerrainNode.hpp"
#include "StackArray2D.hpp"
#include "HeightSource.hpp"
#include "TerrainQuadTreeConfiguration.hpp"


class NzTerrainQuadTree
{
    public:
        NzTerrainQuadTree(const NzTerrainQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource);
        ~NzTerrainQuadTree();

        void DrawTerrain();
        const std::list<NzTerrainNode*>& GetLeavesList();
        void Initialize(const NzVector3f& cameraPosition);
        NzTerrainNode* GetNode(id nodeID);
        NzTerrainNode* GetRootPtr();

        //FIX ME : ces 3 m�thodes doit �tre private et NzTerrainNode ajout� en friend
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont m�me peut �tre inutiles, y compris maintenir � jour m_leaves
        //Peut �ventuellement servir � optimiser Update(..)
        void RegisterLeaf(NzTerrainNode* node);
        bool UnRegisterLeaf(NzTerrainNode* node);
        bool UnRegisterNode(NzTerrainNode* node);

        //Updates the terrain mesh accordingly to the camera position
            //If you want a pure static terrain, you must not call this function
            //Otherwise this function will makes the terrain closer to the camera more precise
        void Update(const NzVector3d& cameraPosition);


    private:
        NzTerrainNode* root;
        //Ces listes n'ont pas la charge des objets en m�moire
        std::list<NzTerrainNode*> m_leaves;
        StackArray2D<NzTerrainNode*> m_nodes;
        NzHeightSource* m_heightSource;

        NzTerrainQuadTreeConfiguration m_configuration;
        unsigned int m_buffersAmount;

        bool m_isInitialized;
};

#endif // QUADTREE_HPP
