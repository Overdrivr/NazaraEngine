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
#include "TerrainNode.hpp"
#include "HeightSource.hpp"
#include "TerrainQuadTreeConfiguration.hpp"
#include "Dispatcher.hpp"

class NzTerrainQuadTree
{
    public:
        NzTerrainQuadTree(const NzTerrainQuadTreeConfiguration& configuration, const NzVector2f& terrainCenter, NzHeightSource* heightSource);
        ~NzTerrainQuadTree();

        const std::list<NzTerrainNode*>& GetLeavesList();
        float GetMaximumHeight() const;
        unsigned int GetSubdivisionsAmount();

        NzTerrainNode* GetNode(id nodeID);
        NzTerrainNode* GetRootPtr();

        NzTerrainNode* GetNodeFromPool();
        void ReturnNodeToPool(NzTerrainNode* node);

        void Initialize(const NzVector3f& cameraPosition);

        void Render();

        //FIX ME : ces 3 méthodes doit être private et NzTerrainNode ajouté en friend
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont même peut être inutiles, y compris maintenir à jour m_leaves
        //Peut éventuellement servir à optimiser Update(..)
        void RegisterLeaf(NzTerrainNode* node);
        bool UnRegisterLeaf(NzTerrainNode* node);//Les feuilles enlevées ici doivent aussi l'être de la camera list
        bool UnRegisterNode(NzTerrainNode* node);
        //Updates the terrain mesh accordingly to the camera position
            //If you want a pure static terrain, you must not call this function
            //Otherwise this function will makes the terrain closer to the camera more precise
        void Update(const NzVector3f& cameraPosition);
        void AddLeaveToSubdivisionList(NzTerrainNode* node);

        //Returns -1 if the distance to the camera is too big
        //or the radius index otherwise
        int TransformDistanceToCameraInRadiusIndex(float distance);
    private:
        NzTerrainNode* m_root;
        TerrainNodeData m_data;

        std::list<NzTerrainNode*> m_leaves;//Inutilisé?

        std::map<id,NzTerrainNode*> m_nodesMap;

        std::map<id,NzTerrainNode*> m_cameraList;
        std::map<id,NzTerrainNode*> m_subdivideList;
        std::map<id,NzTerrainNode*> m_removeList;

        unsigned int m_currentCameraRadiusIndex;

        unsigned int m_subdivisionsAmount;
        unsigned int m_poolReallocationSize;
        unsigned int m_poolAllocatedSpace;

        NzHeightSource* m_heightSource;

        NzTerrainQuadTreeConfiguration m_configuration;
        unsigned int m_buffersAmount;

        bool m_isInitialized;

        unsigned int m_maxOperationsPerFrame;
};

#endif // QUADTREE_HPP
