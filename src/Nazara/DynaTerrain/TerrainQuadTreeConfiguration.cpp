// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTreeConfiguration.hpp>
#include <Nazara/Math/Rect.hpp>
#include <cmath>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainQuadTreeConfiguration::NzTerrainQuadTreeConfiguration()
{
    //Taille d'un côté du terrain
    terrainSize = 2000.f;
    //La hauteur maximale du terrain (sous condition que la source de hauteur renvoie des valeurs dans [0;1]
    terrainHeight = 100.f;
    //La précision minimale du terrain
    minimumDepth = 2;
    //La précision maximale engendrée par la plus forte variation de pente
    slopeMaxDepth = 4;
    //La précision au plus près de la caméra
    closeCameraDepth = 7;
    //La précision au plus loin de la caméra
    farCameraDepth = 4;
    //La distance sur laquelle la précision de la caméra va décroitre
    effectRadius = 300.f;
    //Le rayon de précision maximale autour de la caméra (le rayon d'influence de la caméra est donc ici 300 + 30 = 330.f)
    startRadius = 30.f;
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
    return minimumDepth <= slopeMaxDepth &&
           minimumDepth < closeCameraDepth &&
           farCameraDepth < closeCameraDepth &&
           effectRadius > 0.f &&
           effectRadius > startRadius &&
           ComputeCameraSlope() < 1.f;
}
