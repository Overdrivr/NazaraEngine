// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/DynaTerrain/HeightSource.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
#include "HeightSource.hpp"
#include <iostream>
#include <Nazara/Core/File.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

NzHeightSource::NzHeightSource()
{

}
NzHeightSource::~NzHeightSource()
{

}

float NzHeightSource::GetHeight(float x, float y)
{
    //Coder ici la routine permettant de récupérer les données du terrain non procédurales

    return this->GetNoiseValue(x,y);
}

bool NzHeightSource::LoadTerrainFile(const NzString& filename)
{
    m_filepath = filename;
    NzFile file(filename);
    if(!file.Open(0x6))
    {
        std::cout<<"Impossible d'ouvrir le fichier : "<<filename<<std::endl;
        return false;
    }

    file.Read(&m_structureVersion, sizeof(nzUInt8),1);
    if(m_structureVersion == 0x01)
    {
        nzUInt8 properties;
        file.Read(&properties,sizeof(nzUInt8),1);
        m_bicubicInterpolation = properties & 0x01;
        m_proceduralDetails = properties & 0x02;
        file.Read(&m_chunkSize,sizeof(float),1);
        file.Read(&m_chunksNumber,sizeof(nzUInt32),1);
        for(unsigned int i(0) ; i < m_chunksNumber ; ++i)
        {
            NzString temp;
            nzInt32 coordinate;
            file.Read(&coordinate,sizeof(nzInt32),1);
            temp += NzString::Number(coordinate);
            file.Read(&coordinate,sizeof(nzInt32),1);
            temp += NzString::Number(coordinate);
            temp += ".png";
            m_chunkPatches.push_back(temp);
        }
        file.Close();
        std::cout<<"Loaded data for "<<m_chunksNumber<<" chunk(s) with properties : "<<m_proceduralDetails<<"|"<<m_bicubicInterpolation<<std::endl;
        for(unsigned int i(0) ; i < m_chunksNumber ; ++i)
            std::cout<<m_chunkPatches.at(i)<<std::endl;
        return true;
    }
    else
    {
        std::cout<<"Version de structure non supportée : "<<m_structureVersion<<std::endl;
        file.Close();
        return false;
    }
    return false;
}
