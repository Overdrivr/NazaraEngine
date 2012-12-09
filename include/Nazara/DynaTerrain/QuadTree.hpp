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
#include "Node.hpp"
#include "StackArray2D.hpp"
#include "HeightSource.hpp"
#include "NzQuadTreeConfiguration.hpp"


class NzQuadTree
{
    public:
        NzQuadTree(const NzQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource);
        ~NzQuadTree();

        void DrawTerrain();
        const std::list<NzNode*>& GetLeavesList();
        void Initialize(const NzVector3f& cameraPosition);
        NzNode* GetNode(id nodeID);
        NzNode* GetRootPtr();

        //FIX ME : ces 3 m�thodes doit �tre private et NzNode ajout� en friend
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont m�me peut �tre inutiles, y compris maintenir � jour m_leaves
        //Peut �ventuellement servir � optimiser Update(..)
        void RegisterLeaf(NzNode* node);
        bool UnRegisterLeaf(NzNode* node);
        bool UnRegisterNode(NzNode* node);

        //Updates the terrain mesh accordingly to the camera position
            //If you want a pure static terrain, you must not call this function
            //Otherwise this function will makes the terrain closer to the camera more precise
        void Update(const NzVector3d& cameraPosition);


    private:
        NzNode* root;
        //Ces listes n'ont pas la charge des objets en m�moire
        std::list<NzNode*> m_leaves;
        StackArray2D<NzNode*> m_nodes;
        NzHeightSource* m_heightSource;

        NzQuadTreeConfiguration m_configuration;
        unsigned int m_buffersAmount;

        bool m_isInitialized;
};

#endif // QUADTREE_HPP
