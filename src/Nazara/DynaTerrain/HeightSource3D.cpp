// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/HeightSource3D.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <iostream>
#include <Nazara/Core/File.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzHeightSource3D::NzHeightSource3D()
{

}
NzHeightSource3D::~NzHeightSource3D()
{

}

float NzHeightSource3D::GetHeight(float x, float y, float z)
{
    //Coder ici la routine permettant de r�cup�rer les donn�es du terrain non proc�durales

    return this->GetNoiseValue(x,y,z);
}
