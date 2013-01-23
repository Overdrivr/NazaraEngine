// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PATCH_HPP
#define NAZARA_PATCH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <array>
#include <Nazara/Math/Cube.hpp>

//FIX ME : Eliminer tableau redondant debug

class NAZARA_API NzPatch
{
    public:
        NzPatch();
        ~NzPatch();

        NzVector2f GetCenter() const;
        float GetSize() const;
        NzCubef& GetAABB();
        const NzCubef& GetAABB() const;
        float GetGlobalSlope() const;

        void Initialize(NzVector2f center, float size, id nodeID, TerrainNodeData* data);
        void Invalidate();

        void SetConfiguration(bool leftNeighbor, bool topNeighbor, bool rightNeighbor, bool bottomNeighbor);

        void UploadMesh();
        void UnUploadMesh();

    protected:
    private:

        void ComputeNormals();
        void ComputeSlope();
        void ComputeHeights();

        TerrainNodeData* m_data;
        id m_id;
        NzVector2f m_center;
        NzCubef m_aabb;
        float m_size;
        unsigned short int m_configuration;
        std::array<float,25> m_noiseValues;
        std::array<float,49> m_extraHeightValues;
        std::array<NzVector3f,25> m_vertexNormals;
        std::array<float,150> m_uploadedData;
        float m_slope;
        bool m_isUploaded;
        bool m_isInitialized;
};

#endif // NAZARA_PATCH_HPP
