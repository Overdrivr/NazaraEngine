// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PATCH_HPP
#define PATCH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
//#include <Nazara/DynaTerrain/NzHeightSource.hpp>
#include "HeightSource.hpp"
#include "Enumerations.hpp"
#include <array>

class NzPatch
{
    public:
        NzPatch(NzVector2f center, NzVector2f size, id nodeID);
        ~NzPatch();
        void ComputeSlope();
        NzVector2f GetCenter() const;
        NzVector2f GetSize() const;
        unsigned int GetTerrainConstrainedMinDepth();
        bool IntersectsCircle(const NzVector2f& center, double radius);//FIX ME : Hériter NzRect
        bool IsContainedByCircle(const NzVector2f& center, double radius);
        void RecoverPatchHeightsFromSource();
        void SetConfiguration(bool leftNeighbor,
                              bool topNeighbor,
                              bool rightNeighbor,
                              bool bottomNeighbor);

        void SetData(TerrainNodeData* data);

        void UploadMesh();
        void UnUploadMesh();

    protected:
    private:
        TerrainNodeData* m_data;
        id m_id;
        NzVector2f m_center;
        NzVector2f m_size;
        unsigned short int m_configuration;
        std::array<float,25> m_noiseValues;
        std::array<float,150> m_uploadedData;
        float m_slope;
        float m_noiseValuesDistance;
        float m_sensitivity;
        bool m_isDataSet;
        bool m_isHeightDefined;
        bool m_isSlopeComputed;
        bool m_isUploaded;
};

#endif // PATCH_HPP
