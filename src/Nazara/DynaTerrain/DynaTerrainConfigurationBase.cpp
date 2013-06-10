// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/DynaTerrainConfigurationBase.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzDynaTerrainConfigurationBase::NzDynaTerrainConfigurationBase()
{
    //La hauteur maximale du terrain/planète (sous condition que la source de hauteur renvoie bien des valeurs dans [0;1])
    maxHeight = 1000.f;
    //La précision minimale du terrain/planète (= le quadtree sera subdivisé équitablement jusqu'à cette profondeur)
    minPrecision = 2;
    //La précision maximale engendrée par la plus forte variation de pente
    maxSlopePrecision = 2;
    //La précision au plus près de la caméra
    higherCameraPrecision = 8;
    //Le nombre de cercles de précision autour de la caméra
    cameraRadiusAmount = 6;
    //Le rapport de taille entre deux rayons consécutifs
    radiusSizeIncrement = 2.5f;
    //Le rayon de précision maximale autour de la caméra
    higherCameraPrecisionRadius = 75.f;
    //Le chemin vers la texture du terrain
    groundTextures = "unset_path_to_terrain_texture_file/unset_terrain_texture.jpg";

    x_offset = 0;
    y_offset = 0;
}

NzDynaTerrainConfigurationBase::~NzDynaTerrainConfigurationBase()
{
    //dtor
}

void NzDynaTerrainConfigurationBase::AutoFix()
{
    if(cameraRadiusAmount == 0)
        cameraRadiusAmount = 1;

    if(radiusSizeIncrement <= 0.f)
        radiusSizeIncrement = 1.2f;
}

bool NzDynaTerrainConfigurationBase::IsValid() const
{
    if(cameraRadiusAmount == 0)
        return false;

    if(radiusSizeIncrement <= 0.f)
        return false;

    return true;
}


