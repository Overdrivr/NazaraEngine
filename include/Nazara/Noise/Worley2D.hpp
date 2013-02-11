// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef WORLEY2D_HPP
#define WORLEY2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <random>
#include <map>

class NAZARA_API NzWorley2D : public NzAbstract2DNoise
{
    public:
        NzWorley2D(worleyFunction function = F1);
        NzWorley2D(unsigned int seed);

        float GetValue(float x, float y, float resolution);
        //FIX ME : Rajouter fonction pour récupérer l'état complet après un calcul

        void SetFunction(worleyFunction function);
        ~NzWorley2D() = default;
    protected:
    private:
        void SquareTest(int xi, int yi, float x, float y);

        std::map<float,NzVector2f> featurePoints;
        std::map<float,NzVector2f>::iterator it;
        std::minstd_rand0 randomNumberGenerator;
        float scale[4];

        worleyFunction m_function;

        int seed;
        int x0, y0;
        int ii, jj;
        float fractx, fracty;
        /*
        int gi0,gi1,gi2,gi3;
        float gradient2[8][2];
        float s,t,u,v;
        float Cx,Cy;
        float Li1, Li2;
        NzVector2<float> temp;*/
};

#endif // PERLIN2D_HPP

