// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX3D_HPP
#define SIMPLEX3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Math/Vector3.hpp>

class NAZARA_API NzSimplex3D : public NzNoiseBase
{
    public:
        NzSimplex3D();
        NzSimplex3D(unsigned int seed);
        ~NzSimplex3D() = default;

        float Get();

        void Set(float X, float Y, float Z);

    protected:
        float x;
        float y;
        float z;
    private:
        float xc, yc, zc;
        int ii,jj,kk;
        int gi0,gi1,gi2,gi3;
        NzVector3i skewedCubeOrigin,off1,off2;
        float n1,n2,n3,n4;
        float c1,c2,c3,c4;
        float gradient3[12][3];
        float UnskewCoeff3D;
        float SkewCoeff3D;
        float sum;
        NzVector3<float> unskewedCubeOrigin, unskewedDistToOrigin;
        NzVector3<float> d1,d2,d3,d4;
};

#endif // SIMPLEX3D_HPP

