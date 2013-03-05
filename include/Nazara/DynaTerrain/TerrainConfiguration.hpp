// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINCONFIGURATION_HPP
#define NAZARA_TERRAINCONFIGURATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/EulerAngles.hpp>

class NAZARA_API NzTerrainConfiguration
{
    //For now, there are two different mesh-optimizing methods :
        //-Distance to camera
        //-Slope variation
    //Each method accepts various parameters, but there are several conditions over these parameters
    //This class helps configurating the quadtree and checks input parameters
    public:
        NzTerrainConfiguration();
        ~NzTerrainConfiguration();

        void AutoFix();

        bool IsValid() const;

        //Terrain parameters

        float terrainSize;
        float maxTerrainHeight;
        NzEulerAnglesf terrainOrientation;
        NzVector3f terrainCenter;

        unsigned int minTerrainPrecision;

        unsigned int maxSlopePrecision;

        unsigned int higherCameraPrecision;
        float higherCameraPrecisionRadius;
        unsigned int cameraRadiusAmount;
        float radiusSizeIncrement;

        NzString vertexShader;
        NzString fragmentShader;
        NzString terrainTexture;

    protected:
    private:
};

#endif // NAZARA_TERRAINCONFIGURATION_HPP
