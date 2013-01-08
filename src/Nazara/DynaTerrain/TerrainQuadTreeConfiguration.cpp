// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "TerrainQuadTreeConfiguration.hpp"
#include <cmath>

NzTerrainQuadTreeConfiguration::NzTerrainQuadTreeConfiguration()
{
    terrainSize = 2000.f;
    terrainHeight = 100.f;
    minimumDepth = 2;
    slopeMaxDepth = 6;
    closeCameraDepth = 8;
    farCameraDepth = 4;
    effectRadius = 300.f;
}

NzTerrainQuadTreeConfiguration::~NzTerrainQuadTreeConfiguration()
{
    //dtor
}

float NzTerrainQuadTreeConfiguration::ComputeCameraSlope() const
{
    return (terrainSize/std::pow(2,farCameraDepth) - terrainSize/std::pow(2,closeCameraDepth))/effectRadius;
}

unsigned int NzTerrainQuadTreeConfiguration::ComputeMaxPatchNumber() const
{
    unsigned int camera = 0;

    //Pour chaque cercle de changement de profondeur
    //i = 0 : plus grand cercle
    //i = m_closeCameraDepth-m_farCameraDepth-1 : plus petit

    unsigned int nbCercles = closeCameraDepth-farCameraDepth+1;

    for(unsigned int i(0) ; i < nbCercles ; ++i)
    {
        if(i > nbCercles-1)
        {

            float radius = effectRadius/nbCercles;
            float edge = terrainSize/(std::pow(2,closeCameraDepth));
            //On calcule le nombre de patchs dans le cercle le plus petit
            camera += static_cast<unsigned int>(2*radius/edge);
        }
        else
        {
            float radius = effectRadius*(nbCercles-i)/nbCercles;

            float radius2 = effectRadius*(nbCercles-i-1)/nbCercles;

            float edge = terrainSize/(std::pow(2,farCameraDepth+i));
            //On calcule le nombre de patchs entre le cercle i et le cercle i+1
            camera += static_cast<unsigned int>(2*radius/edge) - static_cast<unsigned int>(2*radius2/edge);
        }

    }

    float edge = terrainSize/(std::pow(2,2*(farCameraDepth)));
    unsigned int slope = static_cast<unsigned int>(std::pow(2,2*slopeMaxDepth)) - static_cast<unsigned int>(2*effectRadius/edge);

    return camera + slope;
}

bool NzTerrainQuadTreeConfiguration::IsValid() const
{
    return minimumDepth < slopeMaxDepth &&
           minimumDepth < closeCameraDepth &&
           farCameraDepth < closeCameraDepth &&
           effectRadius > 0.f &&
           ComputeCameraSlope() < 1.f;
}
