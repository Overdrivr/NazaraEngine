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

        //Computes the number of GPU buffers to host the terrain data
            //It is possible to know the maximum number of buffers with the maximum slope contribution and the space-integrated camera contribution
            //returns false if the terrain hasn't been initialized
        bool ComputeGPUBuffersAmount();
        //Dessine le terrain
        void DrawTerrain();
        //Pour obtenir la liste des nodes feuilles (au plus bas de l'arbre)
        const std::list<NzNode*>& GetLeavesList();
        //Pour obtenir un pointeur vers un node d'après son id, renvoie nullptr si le node n'existe pas
        NzNode* GetNode(id nodeID);
        //Renvoie le pointeur racine de l'arbre
        NzNode* GetRootPtr();

        //Initialize the terrain
            ///minimumDepth is the lowest possible level (i.e. precision) for the terrain
            ///slopeContribution is the deepest level the tree can reach in case of big slope variation
            //If slopeContribution = 0, the slope variation will not be taken in account
            ///cameraContribution is the deepest level the tree can reach around the camera
            ///distanceContribution is the maximum distance to the position of the camera over which the mesh will be more precise
            //after that distance, the mesh will be at its lower precision
            ///Contribution from each "optimisation" are not added, nor multiplied. If slopeContrib = 2 && camera = 3, the deepest level you get is 2, and 3 at the closest of the camera
        void Initialize(unsigned int minimumDepth,
                        unsigned int slopeContribution,
                        unsigned int closeCameraContribution, unsigned int farCameraContribution, float distanceContribution,
                        const NzVector3f& cameraPosition = NzVector3f(0.f,0.f,0.f));

        //FIX ME : ces 3 méthodes doit être private et NzNode ajouté en friend
        //Vu que quadtree ne sera pas en charge de l'affichage, elles sont même peut être inutiles, y compris maintenir à jour m_leaves
        //Peut éventuellement servir à optimiser Update(..)
        void RegisterLeaf(NzNode* node);
        bool UnRegisterLeaf(NzNode* node);
        bool UnRegisterNode(NzNode* node);

        //Updates the terrain mesh accordingly to the camera position
            //If you want a pure static terrain, you must not call this function
            //Otherwise this function will makes the terrain closer to the camera more precise
        void Update(const NzVector3d& cameraPosition);


    private:
        NzNode* root;
        //Ces listes n'ont pas la charge des objets en mémoire
        std::list<NzNode*> m_leaves;
        StackArray2D<NzNode*> m_nodes;
        NzHeightSource* m_heightSource;

        unsigned int m_minimumDepth;
        unsigned int m_slopeContribution;
        unsigned int m_closeCameraContribution;
        unsigned int m_farCameraContribution;
        float m_distanceContribution;
        bool m_isInitialized;
        NzVector2f m_terrainSize;

        unsigned int m_buffersAmount;
};

#endif // QUADTREE_HPP
