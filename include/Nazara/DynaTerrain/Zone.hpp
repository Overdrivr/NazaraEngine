// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef ZONE_HPP
#define ZONE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include "Enumerations.hpp"
#include "SparseBuffer.hpp"
#include "SparseBufferSet.hpp"

#include <queue>
#include <array>
#include <vector>

class NzDispatcher;



class NzZone
{
    public:
        NzZone(NzDispatcher* dispatcher);
        ~NzZone();

        void AddBuffer(NzVertexBuffer* buffer);
        void AddPatch(const std::array<float,150>& vertices, const id& ID);

        void DrawBuffers() const;

        unsigned int GetFreeBuffersAmount();
        unsigned int GetFreeSlotsAmount();

        void Optimize(int amount);

        bool RemoveFreeBuffer(NzVertexBuffer* buffer);
        bool RemovePatch(const id& ID);

        bool UpdatePatch(const std::array<float,150>& vertices, const id& ID);

    protected:
    private:


        NzDispatcher* m_dispatcher;
        //Raw data for rendering
        std::vector<NzVertexBuffer*> m_buffers;
        //Image of the raw data for quick search over patch id & memory fragmentation reduction
        NzSparseBufferSet<id> m_buffersMap;

        //Contient l'ensemble des patches qui n'ont pas pu �tre mis en m�moire vid�o pour cause d'espace insuffisant
            //Non utilis� : FIX ME : Vraiment Utile ?
        std::queue<float> m_unbufferedPatches;
        std::queue<id> m_unbufferedPatchesIndex;
};

#endif // ZONE_HPP
