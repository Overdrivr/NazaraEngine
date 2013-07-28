// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINCHUNK_HPP
#define NAZARA_TERRAINCHUNK_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/TerrainRenderer/IntervalBuffer.hpp>
#include <Nazara/TerrainRenderer/SparseBufferSet.hpp>

#include <Nazara/Math/BoundingBox.hpp>
#include <queue>
#include <array>
#include <vector>

// class NzTerrainMasterNode;

class NAZARA_API NzTerrainChunk
{
    friend class NzTerrainRenderer;

    public:
        NzTerrainChunk();
        ~NzTerrainChunk();

        bool AddMesh(const std::array<float,150>& vertexData, const NzBoundingBoxf& meshBoundingBox, NzTerrainNodeID meshIdentifiant);
        bool UpdateMesh(const std::array<float,150>& vertexData,NzTerrainNodeID meshIdentifiant);
        bool RemoveMesh(NzTerrainNodeID meshIdentifiant);

    protected:
    private:
        bool CreateBuffer();

        std::vector<NzVertexBuffer> m_vertexBuffers;
        //IntervalBuffer
        std::vector<NzIntervalBuffer<NzTerrainNodeID>> m_vertexBuffersMap;
        unsigned int m_freeSlotsAmount;
        //BoundingBox
        NzBoundingBoxf m_globalBoundingBox;
        //La déclaration de structure du buffer
        NzVertexDeclaration m_declaration;
};

#endif // NAZARA_TERRAINCHUNK_HPP
