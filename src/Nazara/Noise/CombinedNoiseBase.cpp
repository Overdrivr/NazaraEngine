// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cmath>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/CombinedNoiseBase.hpp>
#include <Nazara/Noise/Debug.hpp>

NzCombinedNoiseBase::NzCombinedNoiseBase()
{
    m_lacunarity = 5.0f;
    m_hurst = 1.2f;
    m_octaves = 3.0f;
}

float NzCombinedNoiseBase::GetLacunarity() const
{

    return m_lacunarity;
}

float NzCombinedNoiseBase::GetHurstParameter() const
{
    return m_hurst;
}

float NzCombinedNoiseBase::GetOctaveNumber() const
{
    return m_octaves;
}

void NzCombinedNoiseBase::SetParameters(float hurst, float lacunarity, float octaves)
{
    m_lacunarity = lacunarity;
    m_hurst = hurst;
    m_octaves = octaves;

    float frequency = 1.0;
    m_sum = 0.f;
    for (int i(0) ; i < static_cast<int>(m_octaves) ; ++i)
    {

        m_exponent_array[i] = std::pow( frequency, -m_hurst );
        frequency *= m_lacunarity;

        m_sum += m_exponent_array[i];

    }
}
