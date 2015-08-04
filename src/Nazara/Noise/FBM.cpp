// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Noise/FBM.hpp>
#include <Nazara/Noise/Debug.hpp>

NzFBM::NzFBM(NzNoiseBase& source): m_source(source)
{

}

float NzFBM::Get()
{
    m_value = 0.0;

    float initialScale = m_source.GetScale();

    for (int i(0); i < m_octaves; ++i)
    {
        m_value += m_source.Get() * m_exponent_array[i];
        m_scale *= m_lacunarity;
        m_source.SetScale(m_scale);
    }
    m_remainder = m_octaves - static_cast<int>(m_octaves);

    if(!NzNumberEquals(m_remainder, static_cast<float>(0.0)))
      m_value += m_remainder * m_source.Get() * m_exponent_array[static_cast<int>(m_octaves-1)];

    m_source.SetScale(initialScale);

    return m_value/m_sum;
}
