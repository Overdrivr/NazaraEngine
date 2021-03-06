// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN2D_HPP
#define PERLIN2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Math/Vector2.hpp>

class NAZARA_API NzPerlin2D : public NzNoiseBase
{
    public:
        NzPerlin2D();
        NzPerlin2D(unsigned int seed);
        ~NzPerlin2D() = default;

        float Get();

        void Set(float X, float Y);

    protected:
        float x;
        float y;
    private:
        float xc, yc;
        int x0, y0;
        int gi0,gi1,gi2,gi3;
        int ii, jj;
        float gradient2[8][2];
        float s,t,u,v;
        float Cx,Cy;
        float Li1, Li2;
        NzVector2<float> temp;
};

#endif // PERLIN2D_HPP

