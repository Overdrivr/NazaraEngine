// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ZONE_HPP
#define NAZARA_ZONE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/DynaTerrain/SparseBuffer.hpp>
#include <Nazara/DynaTerrain/SparseBufferSet.hpp>

#include <queue>
#include <array>
#include <vector>

class NzDispatcher;

class NAZARA_API NzZone
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

        //Contient l'ensemble des patches qui n'ont pas pu être mis en mémoire vidéo pour cause d'espace insuffisant
            //Non utilisé : FIX ME : Vraiment Utile ?
        std::queue<float> m_unbufferedPatches;
        std::queue<id> m_unbufferedPatchesIndex;
};

#endif // NAZARA_ZONE_HPP
