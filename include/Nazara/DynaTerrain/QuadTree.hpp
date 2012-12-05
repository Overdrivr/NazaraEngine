// Copyright (C) 2012 Rémi Bèges
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


class NzQuadTree
{
    public:
        NzQuadTree(const NzVector2f& terrainCenter, const NzVector2f& terrainSize, NzHeightSource* heightSource);
        ~NzQuadTree();

        void DrawTerrain();
        const std::list<NzNode*>& GetLeavesList();
        NzNode* GetNode(id nodeID);
        NzNode* GetRootPtr();
        void Prepare();
        void RegisterLeaf(NzNode* node);
        bool UnRegisterLeaf(NzNode* node);
        bool UnRegisterNode(NzNode* node);
        void Update(const NzVector3d& cameraPosition);

    private:
        NzNode* root;
        //Ces listes n'ont pas la charge des objets en mémoire
        std::list<NzNode*> m_leaves;
        StackArray2D<NzNode*> m_nodes;
        NzHeightSource* m_heightSource;
};

#endif // QUADTREE_HPP
