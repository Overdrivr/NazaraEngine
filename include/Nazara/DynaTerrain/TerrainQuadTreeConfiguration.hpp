// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINQUADTREECONFIGURATION_HPP
#define NAZARA_TERRAINQUADTREECONFIGURATION_HPP

#include <Nazara/Prerequesites.hpp>


class NAZARA_API NzTerrainQuadTreeConfiguration
{
    //For now, there are two different mesh-optimizing methods :
        //-Distance to camera
        //-Slope variation
    //Each method accepts various parameters, but there are several conditions over these parameters
    //This class helps configurating the quadtree and checks input parameters
    public:
        NzTerrainQuadTreeConfiguration();
        ~NzTerrainQuadTreeConfiguration();
        float ComputeCameraSlope() const;
        unsigned int ComputeMaxPatchNumber() const;
        bool IsValid() const;

        //Terrain parameters
        float terrainSize;
        float terrainHeight;
        unsigned int minimumDepth;
        //Distance to camera parameters
        unsigned int closeCameraDepth;
        unsigned int farCameraDepth;
        float effectRadius;
        //Slope variation parameters
        unsigned int slopeMaxDepth;
        float startRadius;

    protected:
    private:
};

#endif // NAZARA_TERRAINQUADTREECONFIGURATION_HPP
