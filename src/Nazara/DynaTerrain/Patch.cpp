// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "Patch.hpp"
//#include <Nazara/DynaTerrain/Patch.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>
#include <cmath>
#include "TerrainQuadTree.hpp"
#include <iostream>
#include "Dispatcher.hpp"

using namespace std;



NzPatch::NzPatch(NzVector2f center, NzVector2f size, id nodeID)
{
    //std::cout<<"Creating patch "<<nodeID.lvl<<"|"<<nodeID.sx<<"|"<<nodeID.sy<<std::endl;
    m_id = nodeID;
    m_center = center;
    m_realCenter.x = center.x;
    m_realCenter.y = center.y;
    m_realCenter.z = 0.f;
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

void NzPatch::ComputeNormals()
{
    //top, right, bottom, left
    NzVector3f v1,v2,v3,v4;
    NzVector3f v12;
    NzVector3f v23;
    NzVector3f v34;
    NzVector3f v41;
    NzVector3f sum;

    unsigned int i0,j0;
    unsigned int i1,j1;

    for(unsigned int i(0) ; i < 5 ; ++i)
        for(unsigned int j(0) ; j < 5 ; ++j)
        {
            i0 = i + 1;
            j0 = j + 1;
            i1 = i - 1;
            j1 = j - 1;
            //Compute four vectors
            v1.x = m_size.x*(0.25*(i0+1)-0.5);
            v1.y = m_size.y*(0.25*j0-0.5);
            v1.z = m_extraHeightValues.at((i0+1) + 7*j0) * m_data->quadtree->GetMaximumHeight();

            v2.x = m_size.x*(0.25*i0-0.5);
            v2.y = m_size.y*(0.25*(j0+1)-0.5);
            v2.z = m_extraHeightValues.at(i0     + 7*(j0+1)) * m_data->quadtree->GetMaximumHeight();

            v3.x = m_size.x*(0.25*(i0-1)-0.5);
            v3.y = m_size.y*(0.25*j0-0.5);
            v3.z = m_extraHeightValues.at((i0-1) + 7*j0) * m_data->quadtree->GetMaximumHeight();

            v4.x = m_size.x*(0.25*i0-0.5);
            v4.y = m_size.y*(0.25*(j0-1)-0.5);
            v4.z = m_extraHeightValues.at(i0     + 7*(j0-1)) * m_data->quadtree->GetMaximumHeight();

            v12 = v1.CrossProduct(v2);
            v23 = v2.CrossProduct(v3);
            v34 = v3.CrossProduct(v4);
            v41 = v4.CrossProduct(v1);

            sum = v12 + v23 + v34 + v41;
            sum.Normalize();

            if(sum.DotProduct(NzVector3f(0.f,0.f,1.f)) < 0)
                sum *= -1;
            /*if(i == 0 || j == 0)
                m_vertexNormals.at(i+5*j) = NzVector3f(1.0,0.0,0.0);
            else*/
                m_vertexNormals.at(i+5*j) = sum;
        }
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

    //FIX ME : Si la pente change brusquement, l'algo ne le détectera pas
    //Besoin d'un algorithme plus avancé de calcul de variation de pente

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

            ///Maximum method
            if(dSlope[0] > maxdSlope)
                maxdSlope = dSlope[0];
            if(dSlope[1] > maxdSlope)
                maxdSlope = dSlope[1];
            ///Average method
            /*maxdSlope += dSlope[0];
            maxdSlope += dSlope[1];*/
        }
        //maxdSlope /= 50;
        m_slope = maxdSlope;
       /* if(//m_center.x > 700.f && m_center.x < 1000.f &&
           m_center.y > 700.f && m_center.y < 750.f)
                cout<<"slope : "<<m_center.x<<" : "<<maxdSlope*m_sensitivity<<endl;*/
}

NzVector2f NzPatch::GetCenter() const
{
    return m_center;
}

NzVector2f NzPatch::GetSize() const
{
    return m_size;
}

NzVector3f NzPatch::GetRealCenter() const
{
    return m_realCenter;
}

unsigned int NzPatch::GetTerrainConstrainedMinDepth()
{
    if(m_isDataSet)
    {
        if(!m_isSlopeComputed)
            ComputeSlope();

        return static_cast<unsigned int>(m_slope*m_sensitivity);
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

void NzPatch::RecoverPatchHeightsFromSource()
{
    if(m_data->heightSource != nullptr)
    {
        float average = 0;

        for(int i(0) ; i < 5 ; ++i)
            for(int j(0) ; j < 5 ; ++j)
            {
                m_noiseValues[i+5*j] = m_data->heightSource->GetHeight(m_center.x+m_size.x*(0.25*i-0.5),m_center.y+m_size.y*(0.25*j-0.5));
                average += m_noiseValues[i+5*j];
            }

        average /= 25;
        m_realCenter.z = average;

        for(int i(-1) ; i < 6 ; ++i)
            for(int j(-1) ; j < 6 ; ++j)
                m_extraHeightValues.at((i+1)+7*(j+1)) = m_data->heightSource->GetHeight(m_center.x+m_size.x*(0.25*i-0.5),m_center.y+m_size.y*(0.25*j-0.5));

        m_isHeightDefined = true;

        this->ComputeNormals();
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
    for(int i(0) ; i < 5 ; ++i)
        for(int j(0) ; j < 5 ; ++j)
        {
            //Position
            m_uploadedData.at((5*i+j)*6) = m_center.x + m_size.x*(0.25*j-0.5);//X
            m_uploadedData.at((5*i+j)*6+1) = m_noiseValues.at(5*i+j) * m_data->quadtree->GetMaximumHeight();;//Z
            m_uploadedData.at((5*i+j)*6+2) = m_center.y+m_size.y*(0.25*i-0.5);//Y
            //Normales

            m_uploadedData.at((5*i+j)*6+3) = m_vertexNormals.at(5*i+j).x;
            m_uploadedData.at((5*i+j)*6+4) = m_vertexNormals.at(5*i+j).z;
            m_uploadedData.at((5*i+j)*6+5) = m_vertexNormals.at(5*i+j).y;
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

