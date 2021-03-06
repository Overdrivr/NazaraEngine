// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/HybridMultiFractal.hpp>
#include <Nazara/Noise/Debug.hpp>

NzHybridMultiFractal::NzHybridMultiFractal(NzNoiseBase& source) : m_source(source)
{
}

float NzHybridMultiFractal::Get()
{
    m_offset = 1.0f;
    m_value = (m_source.Get() + m_offset) * m_exponent_array[0];
    m_weight = m_value;
    m_signal = 0.f;
    float initialScale = m_source.GetScale();

    m_scale *= m_lacunarity;

    for(int i(1) ; i < m_octaves; ++i)
    {
        if (m_weight > 1.f)
            m_weight = 1.f;

        m_signal = (m_source.Get() + m_offset) * m_exponent_array[i];
        m_value += m_weight * m_signal;

        m_weight *= m_signal;

        m_scale *= m_lacunarity;
        m_source.SetScale(m_scale);
    }

    m_remainder = m_octaves - static_cast<int>(m_octaves);

    if (m_remainder > 0.f)
        m_value += m_remainder * m_source.Get() * m_exponent_array[static_cast<int>(m_octaves-1)];

    m_source.SetScale(initialScale);
    return m_value/m_sum - m_offset;
}
