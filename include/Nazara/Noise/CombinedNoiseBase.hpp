// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef COMBINEDNOISEBASE_HPP
#define COMBINEDNOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <array>

class NAZARA_API NzCombinedNoiseBase : public NzNoiseBase
{
    public:
        NzCombinedNoiseBase();
        ~NzCombinedNoiseBase() = default;

        float GetHurstParameter() const;
        float GetLacunarity() const;
        float GetOctaveNumber() const;
        void SetParameters(float hurst, float lacunarity, float octaves);

    protected:
        float m_lacunarity;
        float m_hurst;
        float m_octaves;
        std::vector<float> m_exponent_array;
        float m_sum;
};

#endif // COMBINEDNOISEBASE_HPP
