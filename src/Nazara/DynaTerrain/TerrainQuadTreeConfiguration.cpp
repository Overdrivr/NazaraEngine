// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTreeConfiguration.hpp>
#include <Nazara/Math/Rect.hpp>
#include <iostream>
#include <cmath>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainQuadTreeConfiguration::NzTerrainQuadTreeConfiguration()
{
    //Taille d'un côté du terrain
    terrainSize = 2000.f;
    //La hauteur maximale du terrain (sous condition que la source de hauteur renvoie des valeurs dans [0;1]
    maxTerrainHeight = 100.f;
    //La précision minimale du terrain
    minTerrainPrecision = 2;
    //La précision maximale engendrée par la plus forte variation de pente
    maxSlopePrecision = 4;
    //La précision au plus près de la caméra
    higherCameraPrecision = 7;
    //Le nombre de cercles de précision autour de la caméra
    cameraRadiusAmount = 4;
    //Le rapport de taille entre deux rayons consécutifs
    radiusSizeIncrement = 2.5f;
    //Le rayon de précision maximale autour de la caméra
    higherCameraPrecisionRadius = 50.f;
}

NzTerrainQuadTreeConfiguration::~NzTerrainQuadTreeConfiguration()
{
    //dtor
}
/*
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
}*/

bool NzTerrainQuadTreeConfiguration::IsValid() const
{
    if(radiusSizeIncrement < 2.f)
        return false;
}
