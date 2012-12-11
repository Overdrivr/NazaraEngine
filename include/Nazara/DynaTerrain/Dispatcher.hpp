// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
//#include <Nazara/Utility/VertexElement.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include "Zone.hpp"
#include "Enumerations.hpp"
#include <vector>
#include <queue>

class NzDispatcher
{
    public:
        NzDispatcher();
        ~NzDispatcher();
        void DrawAll();
        unsigned int GetFreeBuffersAmount() const;
        void ErasePatch(const id& ID);
        //Crée l'ensemble des zones nécessaires à l'affichage
            //Le nombre total de zones est 2^(2*zoneDepth)
            //Alloue aussi le nombre de buffers nécessaires (calculé par NzQuadtree)
        void Initialize(unsigned int zoneDepth, unsigned int bufferAmount);
        //Demande de fournir un buffer libre - toujours observer la valeur renvoyée !
        bool QueryFreeBuffer(NzVertexBuffer* buffer);
        //Retourne un buffer libre
        NzVertexBuffer* ReturnBuffer();
        //Ajoute un patch si possible
        bool SubmitPatch(const std::array<float,150>& subBuffer, const id& ID);
        //Met à jour un patch
        bool UpdatePatch(const std::array<float,150>& subBuffer, const id& ID);
        //Met à jour le view frustum culling
        void ViewFrustumCulling();


    protected:
    private:
        bool m_isReady;
        unsigned int m_zoneDepth;
        unsigned int m_freeBuffersAmount;
        unsigned int m_totalBuffersAmount;
        //Contient l'ensemble des zones
        std::vector<NzZone*> m_zones;
        //Contient l'ensemble des subbuffers
        std::vector<NzVertexBuffer*> m_buffers;
        //Contient les buffers libres
        std::queue<NzVertexBuffer*> m_freeBuffers;

        NzVertexElement m_elements[2];
        //NzVertexDeclaration m_declaration;
        //NzIndexBuffer m_indexBuffer;

};

#endif // DISPATCHER_HPP
