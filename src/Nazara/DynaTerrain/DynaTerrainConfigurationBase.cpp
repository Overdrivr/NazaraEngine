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
    maxHeight = 100.f;
    //Le centre du terrain/plan�te
    center = NzVector3f(0.f,0.f,0.f);
    //La pr�cision minimale du terrain/plan�te (= le quadtree sera subdivis� �quitablement jusqu'� cette profondeur)
    minPrecision = 2;
    //La pr�cision maximale engendr�e par la plus forte variation de pente
    maxSlopePrecision = 4;
    //La pr�cision au plus pr�s de la cam�ra
    higherCameraPrecision = 7;
    //Le nombre de cercles de pr�cision autour de la cam�ra
    cameraRadiusAmount = 4;
    //Le rapport de taille entre deux rayons cons�cutifs
    radiusSizeIncrement = 2.5f;
    //Le rayon de pr�cision maximale autour de la cam�ra
    higherCameraPrecisionRadius = 50.f;
    //Le chemin vers le fichier du vertex shader
    vertexShader = "unset_path_to_vertex_shader_file/unset_vertex_shader.vert";
    //Le chemin vers le fichier du fragment shader
    fragmentShader = "unset_path_to_fragment_shader_file/unset_fragment_shader.frag";
    //Le chemin vers la texture du terrain
    groundTextures = "unset_path_to_terrain_texture_file/unset_terrain_texture.jpg";
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


