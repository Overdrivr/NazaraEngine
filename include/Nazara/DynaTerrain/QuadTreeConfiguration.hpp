// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef QUADTREECONFIGURATION_HPP
#define QUADTREECONFIGURATION_HPP

#include <Nazara/Prerequesites.hpp>


class NzQuadTreeConfiguration
{
    //For now, there are two different mesh-optimizing methods :
        //-Distance to camera
        //-Slope variation
    //Each method accepts various parameters, but there are several conditions over these parameters
    //This class helps configurating the quadtree and checks input parameters
    public:
        NzQuadTreeConfiguration();
        ~NzQuadTreeConfiguration();
        float ComputeCameraSlope() const;
        unsigned int ComputeMaxPatchNumber() const;
        bool IsValid() const;

        //Terrain parameters
        float terrainSize;
        unsigned int minimumDepth;
        //Distance to camera parameters
        unsigned int closeCameraDepth;
        unsigned int farCameraDepth;
        float effectRadius;
        //Slope variation parameters
        unsigned int slopeMaxDepth;

    protected:
    private:
};

#endif // QUADTREECONFIGURATION_HPP
