// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef WORLEY2D_HPP
#define WORLEY2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Enums.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <random>
#include <map>

class NAZARA_API NzWorley2D : public NzNoiseBase
{
    public:
        NzWorley2D(nzWorleyFunction function = nzWorleyFunction_F1);
        NzWorley2D(unsigned int seed);
        ~NzWorley2D() = default;

        float Get();
        //FIX ME : Rajouter fonction pour récupérer l'état complet après un calcul

        void Set(float X, float Y);
        void SetFunction(nzWorleyFunction function);

    protected:
        float x;
        float y;
    private:
        void SquareTest(int xi, int yi, float x, float y);

        std::map<float,NzVector2f> featurePoints;
        std::map<float,NzVector2f>::iterator it;
        std::minstd_rand0 randomNumberGenerator;
        float scale[4];
        nzWorleyFunction m_function;

        float xc, yc;
        int seed;
        int x0, y0;
        int ii, jj;
        float fractx, fracty;
};

#endif // WORLEY2D_HPP

