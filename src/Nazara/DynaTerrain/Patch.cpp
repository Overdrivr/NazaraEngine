// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "Patch.hpp"
//#include <Nazara/DynaTerrain/Patch.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>
#include <cmath>
#include <iostream>
#include "Dispatcher.hpp"

using namespace std;



NzPatch::NzPatch(NzVector2f center, NzVector2f size, id nodeID)
{
    m_id = nodeID;
    m_center = center;
    m_size = size;
    m_data = nullptr;

    m_isDataSet = false;
    m_isHeightDefined = false;
    m_isSlopeComputed = false;
    m_isUploaded = false;

    m_slope = 0.f;
    m_sensitivity = 3;

    m_noiseValuesDistance = size.x / 5.f;

    for(int i(0) ; i < 25 ; ++i)
        m_noiseValues[i] = 0.f;
}

NzPatch::~NzPatch()
{
    //dtor
}

void NzPatch::ComputeSlope()
{
    if(!m_isHeightDefined)
        RecoverPatchHeightsFromSource();

    NzVector2f offset;
    float heightSamples[4];
    float slope[4];
    float dSlope[2];

    float maxdSlope = 0.f;

    for(unsigned int i(0) ; i < 5 ; ++i)
        for(unsigned int j(0) ; j < 5 ; ++j)
        {
        //Pour chaque point on calcule 4 pentes
            //On recupère 4 points autour du point à m_size/20 de distance
            offset.x = 0.f;
            offset.y = 0.f;
            //On veut successivement les offsets suivants :
                //-x  0
                // x  0
                // 0 -y
                // 0  y
            for(unsigned int k(0) ; k < 2 ; ++k)
            {
                offset.x = (2*k-1)*m_size.x/20.f;
                heightSamples[k] = m_data->heightSource->GetHeight(m_center.x+m_size.x*(0.25*i-0.5)+offset.x,
                                                             m_center.y+m_size.y*(0.25*j-0.5)+offset.y);
            }
            offset.x = 0.f;
            for(unsigned int k(0) ; k < 2 ; ++k)
            {
                offset.y = (2*k-1)*m_size.y/20.f;
                heightSamples[k+2] = m_data->heightSource->GetHeight(m_center.x+m_size.x*(0.25*i-0.5)+offset.x,
                                                               m_center.y+m_size.y*(0.25*j-0.5)+offset.y);
            }

            //On calcule les pentes selon x
            slope[0] = (heightSamples[0] - m_noiseValues[i+5*j])/(heightSamples[0] + m_noiseValues[i+5*j]);
            slope[1] = (m_noiseValues[i+5*j] - heightSamples[1])/(heightSamples[1] + m_noiseValues[i+5*j]);
            //On calcule les pentes selon y
            slope[2] = (heightSamples[0] - m_noiseValues[i+5*j])/(heightSamples[2] + m_noiseValues[i+5*j]);
            slope[3] = (m_noiseValues[i+5*j] - heightSamples[1])/(heightSamples[3] + m_noiseValues[i+5*j]);
            //On calcule la variation de pente selon x
            dSlope[0] = std::fabs(slope[1] - slope[0])/std::fabs(slope[1] + slope[0]);
            //On calcule la variation de pente selon y1
            dSlope[1] = std::fabs(slope[3] - slope[2])/std::fabs(slope[3] + slope[2]);

            if(dSlope[0] > maxdSlope)
                maxdSlope = dSlope[0];
            if(dSlope[1] > maxdSlope)
                maxdSlope = dSlope[1];
        }
        m_slope = maxdSlope;
        //cout<<"slope : "<<maxdSlope*m_sensitivity<<endl;
}

NzVector2f NzPatch::GetCenter() const
{
    return m_center;
}

NzVector2f NzPatch::GetSize() const
{
    return m_size;
}

unsigned int NzPatch::GetTerrainConstrainedMinDepth()
{
    if(m_isDataSet)
    {
        if(!m_isSlopeComputed)
            ComputeSlope();

        return static_cast<int>(m_slope*m_sensitivity);
    }
    else
        return 0;
}

void NzPatch::SetConfiguration(bool leftNeighbor, bool topNeighbor, bool rightNeighbor, bool bottomNeighbor)
{
    m_configuration = 0;

    if(leftNeighbor)
        m_configuration += 1;
    if(topNeighbor)
        m_configuration += 1<<1;
    if(rightNeighbor)
        m_configuration += 1<<2;
    if(bottomNeighbor)
        m_configuration += 1<<3;
}

void NzPatch::SetData(TerrainNodeData* data)
{
    m_data = data;
    m_isDataSet = true;
}

bool NzPatch::IntersectsCircle(const NzVector2f& center, double radius)
{
    //A faire
    return false;
}

bool NzPatch::IsContainedByCircle(const NzVector2f& center, double radius)
{
    //A faire
    return false;
}

void NzPatch::RecoverPatchHeightsFromSource()
{
    if(m_data->heightSource != nullptr)
    {
        for(int i(0) ; i < 5 ; ++i)
            for(int j(0) ; j < 5 ; ++j)
                m_noiseValues[i+5*j] = m_data->heightSource->GetHeight(m_center.x+m_size.x*(0.25*i-0.5),m_center.y+m_size.y*(0.25*j-0.5));

        m_isHeightDefined = true;
    }
    else
    {
        for(int i(0) ; i < 5 ; ++i)
            for(int j(0) ; j < 5 ; ++j)
                m_noiseValues[i+5*j] = 0;
        m_isHeightDefined = false;
    }
}

void NzPatch::UploadMesh()
{
    //cout<<"Patch"<<endl;
    for(int i(0) ; i < 5 ; ++i)
        for(int j(0) ; j < 5 ; ++j)
        {
            //cout<<"pos "<<5*i+j<<": "<<m_center.x + m_size.x*(0.25*j-0.5)<<" | "<<m_center.y+m_size.y*(0.25*i-0.5)<<endl;
            //Position
            m_uploadedData.at((5*i+j)*6) = m_center.x + m_size.x*(0.25*j-0.5);//X
            m_uploadedData.at((5*i+j)*6+1) = m_noiseValues.at(5*i+j) * 50.f;//Z
            m_uploadedData.at((5*i+j)*6+2) = m_center.y+m_size.y*(0.25*i-0.5);//Y
            //Normales

            m_uploadedData.at((5*i+j)*6+3) = 1.f;
            m_uploadedData.at((5*i+j)*6+4) = 0.f;
            m_uploadedData.at((5*i+j)*6+5) = 0.f;
        }
    //Le patch classique (une grille carrée de triangles) est constitué de 32 triangles et 25 vertices
    //Mais avec ce patch problèmes aux jonctions entre niveaux. Pour ça, on utilise un patch variable selon les niveaux des patchs voisins
    m_data->dispatcher->SubmitPatch(m_uploadedData,m_id);
    m_isUploaded = true;
    //FIX ME : implementer patch variable
    /*switch(m_configuration)
    {
        case 0 :

        break;

        case 1 :

        break;
    }*/
}

void NzPatch::UnUploadMesh()
{
    if(m_isUploaded)
    {
        //FIX ME : Suppression pas encore suffisamment débuggée pour sparse buffer
        //std::cout<<"Removing patch "<<m_id.lvl<<"|"<<m_id.sx<<"|"<<m_id.sy<<std::endl;
        m_data->dispatcher->RemovePatch(m_id);
        m_isUploaded = false;
    }
}

