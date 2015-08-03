// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NOISEBASE_HPP
#define NOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <random>


class NAZARA_API NzNoiseBase
{
    public:
        NzNoiseBase(unsigned int seed = 0);
        ~NzNoiseBase() = default;

        virtual float Get() = 0;

        float GetScale();

        void SetSeed(unsigned int seed);

        void SetScale(float scale);

        void Shuffle();

        void Shuffle(unsigned int amount);

    protected:
        unsigned int perm[512];
        float m_scale;
    private:
        std::default_random_engine generator;

};

#endif // NOISEBASE_HPP
