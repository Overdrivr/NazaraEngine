// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include "Zone.hpp"
#include "Enumerations.hpp"
#include <map>
#include <vector>
#include <queue>

class NzDispatcher
{
    public:
        NzDispatcher();
        //Provoque le rendu de tous les buffers
        void DrawAll();
        //Enleve un patch de la mémoire
        void ErasePatch(const id& ID);
        //Crée l'ensemble des zones nécessaires à l'affichage
            //Le nombre total de zones est 2^(2*zoneDepth)
            //Alloue aussi le nombre de buffers nécessaires (calculé par NzQuadtree)
        void Prepare(unsigned int zoneDepth, unsigned int numberOfBuffers);
        //Ajoute/Met à jour un patch
        void SubmitPatch(const std::array<float,150>& buffer, const id& ID);
        //Met à jour le view frustum culling
        void ViewFrustumCulling();
        //
        ~NzDispatcher();
    protected:
    private:
        bool m_isReady;
        unsigned int m_zoneDepth;
        //Contient l'ensemble des zones
        std::map<id,NzZone*> m_zones;
        //Contient l'ensemble des buffers
        std::vector<NzVertexBuffer*> m_buffers;
        //Contient les buffers libres
        std::queue<NzVertexBuffer*> m_freeBuffers;

};

#endif // DISPATCHER_HPP
