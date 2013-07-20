// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINNODE_HPP
#define NAZARA_TERRAINNODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/DynaTerrain/IntervalBuffer.hpp>
#include <Nazara/DynaTerrain/SparseBufferSet.hpp>

#include <queue>
#include <array>
#include <vector>

class NzTerrainMasterNode;

///The Terrain Node dedicated purely to display, and interfaced with the scene manager

class NAZARA_API NzTerrainNode
{
    public:
        NzTerrainNode(NzTerrainMasterNode* dispatcher, unsigned int freeSpotsAmount);
        ~NzTerrainNode();

        void AddBuffer(NzVertexBuffer* buffer);
        //FIX ME : Rename into AddMesh
        void AddPatch(const std::array<float,150>& vertices, const NzTerrainNodeID& ID);

        void DrawBuffers() const;

        unsigned int GetFreeBuffersAmount();
        unsigned int GetFreeSlotsAmount();

        void Optimize(int amount);

        bool RemoveFreeBuffer(NzVertexBuffer* buffer);
        bool RemovePatch(const NzTerrainNodeID& ID);

        bool UpdatePatch(const std::array<float,150>& vertices, const NzTerrainNodeID& ID);

    protected:
    private:


        NzTerrainMasterNode* m_dispatcher;
        //Raw data for rendering
        std::vector<NzVertexBuffer*> m_buffers;
        //Image of the raw data for quick search over patch id & memory fragmentation reduction
        NzSparseBufferSet<NzTerrainNodeID> m_buffersMap;

        //Contient l'ensemble des patches qui n'ont pas pu être mis en mémoire vidéo pour cause d'espace insuffisant
            //Non utilisé : FIX ME : Vraiment Utile ?
        std::queue<float> m_unbufferedPatches;
        std::queue<NzTerrainNodeID> m_unbufferedPatchesIndex;
        unsigned int m_freeSpotsAmount;
};

#endif // NAZARA_TERRAINNODE_HPP
