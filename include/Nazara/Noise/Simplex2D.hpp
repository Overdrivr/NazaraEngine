// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX2D_HPP
#define SIMPLEX2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Math/Vector2.hpp>

class NAZARA_API NzSimplex2D : public NzNoiseBase
{
    public:
        NzSimplex2D();
        NzSimplex2D(unsigned int seed);
        virtual ~NzSimplex2D() = default;

        float Get();

        void Set(float X, float Y);

    protected:
        float x;
        float y;
    private:
        float xc,yc;
        int ii,jj;
        int gi0,gi1,gi2;
        NzVector2i skewedCubeOrigin,off1;
        float n1,n2,n3;
        float c1,c2,c3;
        float gradient2[8][2];
        float UnskewCoeff2D;
        float SkewCoeff2D;
        float sum;
        NzVector2<float> unskewedCubeOrigin, unskewedDistToOrigin;
        NzVector2<float> d1,d2,d3;
};

#endif // SIMPLEX2D_HPP

