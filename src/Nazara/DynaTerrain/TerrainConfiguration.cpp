// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainConfiguration::NzTerrainConfiguration()
{
    //Taille d'un c�t� du terrain
    terrainSize = 2000.f;
    //La hauteur maximale du terrain (sous condition que la source de hauteur renvoie des valeurs dans [0;1]
    maxTerrainHeight = 100.f;
    //Le vector d'orientation du terrain. (0,1,0) positionne le terrain naturellement vers le haut
    terrainOrientation = NzVector3f(0.f,1.f,0.f);
    //Le centre du terrain
    terrainCenter = NzVector3f(0.f,0.f,0.f);
    //La pr�cision minimale du terrain
    minTerrainPrecision = 2;
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
    vertexShader;
    //Le chemin vers le fichier du fragment shader
    fragmentShader;
    //Le chemin vers la texture du terrain
    terrainTexture;
}

NzTerrainConfiguration::~NzTerrainConfiguration()
{
    //dtor
}

bool NzTerrainConfiguration::IsValid() const
{
    if(cameraRadiusAmount == 0)
        return false;

    if(radiusSizeIncrement <= 0.f)
        return false;

    return true;
}
