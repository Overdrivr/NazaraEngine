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

class NzPatch
{
    public:
        NzPatch(NzVector2f center, NzVector2f size);
        ~NzPatch();
        void ComputeSlope();
        NzVector2f GetCenter() const;
        NzVector2f GetSize() const;
        unsigned int GetTerrainConstrainedMinDepth();
        bool IntersectsCircle(const NzVector2f& center, double radius);
        bool IsContainedByCircle(const NzVector2f& center, double radius);
        void RecoverPatchHeightsFromSource();
        void SetConfiguration(bool leftNeighbor,
                              bool topNeighbor,
                              bool rightNeighbor,
                              bool bottomNeighbor);

        void SetHeightSource(NzHeightSource* heightSource);

        void UploadMesh();

    protected:
    private:

        NzVector2f m_center;
        NzVector2f m_size;
        unsigned short int m_configuration;
        NzHeightSource* m_heightSource;
        float m_noiseValues[25];
        float m_slope;
        float m_noiseValuesDistance;
        float m_sensitivity;
        bool m_isHeightSourceSet;
        bool m_isHeightDefined;
        bool m_isSlopeComputed;
};

#endif // PATCH_HPP
