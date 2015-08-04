// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

NzNoiseBase::NzNoiseBase(unsigned int seed)
{
    SetSeed(seed);
}

float NzNoiseBase::GetScale()
{
    return m_scale;
}

void NzNoiseBase::SetScale(float scale)
{
    m_scale = scale;
}

void NzNoiseBase::SetSeed(unsigned int seed)
{
    generator.seed(seed);
}

void NzNoiseBase::Shuffle()
{
    int xchanger;
    unsigned int ncase;

    for(unsigned int i(0) ; i < 256 ; i++)
        perm[i] = i;

    for (unsigned int i(0); i < 256 ; ++i)
    {
        ncase = generator() & 255;
        xchanger = perm[i];
        perm[i] = perm[ncase];
        perm[ncase] = xchanger;
    }

    for(unsigned int i(256) ; i < 512; ++i)
        perm[i] = perm[i & 255];
}

void NzNoiseBase::Shuffle(unsigned int amount)
{
    for(unsigned int j(0) ; j < amount ; ++j)
        Shuffle();
}
