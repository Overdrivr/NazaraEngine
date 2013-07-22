// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINCHUNK_HPP
#define NAZARA_TERRAINCHUNK_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/DynaTerrain/IntervalBuffer.hpp>
#include <Nazara/DynaTerrain/SparseBufferSet.hpp>

#include <queue>
#include <array>
#include <vector>

// class NzTerrainMasterNode;

class NAZARA_API NzTerrainChunk
{
    public:
        NzTerrainChunk();
        ~NzTerrainChunk();

        bool AddMesh();
        bool UpdateMesh();
        bool RemoveMesh();

    protected:
    private:
        std::vector<NzVertexBuffer> m_vertexBuffers;
        //IntervalBuffer
        //BoundingBox
};

#endif // NAZARA_TERRAINCHUNK_HPP
