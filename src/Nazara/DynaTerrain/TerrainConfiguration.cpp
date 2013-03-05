// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainConfiguration::NzTerrainConfiguration()
{
    //Taille d'un côté du terrain
    terrainSize = 2000.f;
    //La hauteur maximale du terrain (sous condition que la source de hauteur renvoie des valeurs dans [0;1]
    maxTerrainHeight = 100.f;
    //La rotation du terrain dans les trois axes exprimé en angles d'euler
    terrainOrientation = NzEulerAnglesf(0.f,0.f,0.f);
    //Le centre du terrain
    terrainCenter = NzVector3f(0.f,0.f,0.f);
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
    //Le chemin vers le fichier du vertex shader
    vertexShader = "unset_path_to_vertex_shader_file/unset_vertex_shader.vert";
    //Le chemin vers le fichier du fragment shader
    fragmentShader = "unset_path_to_fragment_shader_file/unset_fragment_shader.frag";
    //Le chemin vers la texture du terrain
    terrainTexture = "unset_path_to_terrain_texture_file/unset_terrain_texture.jpg";
}

NzTerrainConfiguration::~NzTerrainConfiguration()
{
    //dtor
}

void NzTerrainConfiguration::AutoFix()
{
    if(cameraRadiusAmount == 0)
        cameraRadiusAmount = 1;

    if(radiusSizeIncrement <= 0.f)
        radiusSizeIncrement = 1.2f;
}

bool NzTerrainConfiguration::IsValid() const
{
    if(cameraRadiusAmount == 0)
        return false;

    if(radiusSizeIncrement <= 0.f)
        return false;

    return true;
}


