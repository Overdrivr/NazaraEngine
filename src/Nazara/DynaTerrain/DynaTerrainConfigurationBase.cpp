// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/DynaTerrainConfigurationBase.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzDynaTerrainConfigurationBase::NzDynaTerrainConfigurationBase()
{
    //La hauteur maximale du terrain/plan�te (sous condition que la source de hauteur renvoie bien des valeurs dans [0;1])
    maxHeight = 1000.f;
    //La pr�cision minimale du terrain/plan�te (= le quadtree sera subdivis� �quitablement jusqu'� cette profondeur)
    minPrecision = 2;
    //La pr�cision maximale engendr�e par la plus forte variation de pente
    maxSlopePrecision = 2;
    //La pr�cision au plus pr�s de la cam�ra
    higherCameraPrecision = 8;
    //Le nombre de cercles de pr�cision autour de la cam�ra
    cameraRadiusAmount = 6;
    //Le rapport de taille entre deux rayons cons�cutifs
    radiusSizeIncrement = 2.5f;
    //Le rayon de pr�cision maximale autour de la cam�ra
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


