// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PATCH_HPP
#define NAZARA_PATCH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <array>
#include <Nazara/Math/Cube.hpp>

class NAZARA_API NzPatch
{
    public:
        NzPatch();
        ~NzPatch();

        NzCubef& GetAABB();
        const NzCubef& GetAABB() const;
        float GetGlobalSlope() const;

        void Initialize(NzTerrainNodeID nodeID, TerrainNodeData* data);
        void Invalidate();

        void SetConfiguration(nzDirection neighborLocation, unsigned int levelDifference, bool autoUpdate = true);

        void UploadMesh(bool firstTime = true);
        void UnUploadMesh();

    protected:
    private:

        void ComputeNormals();
        void ComputeSlope();
        void ComputeHeights();

        TerrainNodeData* m_data;
        NzTerrainNodeID m_id;
        NzCubef m_aabb;
        unsigned short int m_configuration;
        std::array<NzVector3f,49> m_vertexPositions;
        std::array<NzVector3f,25> m_vertexNormals;
        std::array<float,150> m_uploadedData;
        float m_slope;
        bool m_isUploaded;
        bool m_isInitialized;
};

#endif // NAZARA_PATCH_HPP
