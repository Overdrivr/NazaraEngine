// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef ZONE_HPP
#define ZONE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include "Enumerations.hpp"

#include <array>
#include <vector>
#include <deque>
#include <map>
#include <queue>
#include <memory>

class NzDispatcher;

class NzZone
{
    public:
        NzZone(NzDispatcher* dispatcher);
        ~NzZone();

        void AddBuffer(NzVertexBuffer* buffer);
        void AddPatch(const std::array<float,150>& vertices, const id& ID);

        void DrawBuffers();

        unsigned int GetFreeBuffersAmount();
        unsigned int GetFreeSubBuffersAmount();

        bool RemoveFreeBuffer(NzVertexBuffer* buffer);
        bool RemovePatch(const id& ID);

        bool UpdatePatch(const std::array<float,150>& vertices, const id& ID);

    protected:
    private:
        NzDispatcher* m_dispatcher;

        std::vector<NzVertexBuffer*> m_buffers;

        std::deque<nzBufferLocation> m_freeSpotsIndex;

        //Contient l'ensemble des id des patchs contenus par la zone et leur emplacement dans le buffer
        std::map<id,nzBufferLocation> m_patchesIndex;

        //Contient l'ensemble des patches qui n'ont pas pu être mis en mémoire vidéo pour cause d'espace insuffisant
        std::queue<float> m_unbufferedPatches;
        std::queue<id> m_unbufferedPatchesIndex;
};

#endif // ZONE_HPP
