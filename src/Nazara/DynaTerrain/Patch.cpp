// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/Patch.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <cmath>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

NzPatch::NzPatch()
{
    m_isInitialized = false;
    m_slope = 0.f;
}

NzPatch::~NzPatch()
{
    //dtor
}

void NzPatch::ComputeHeights()
{
    if(m_isInitialized)
    {
        float x,z;

        for(int i(0) ; i < 5 ; ++i)
            for(int j(0) ; j < 5 ; ++j)
            {
                x = m_center.x+m_size*(0.25*i-0.5);
                z = m_center.y+m_size*(0.25*j-0.5);

                //We recover y (altitude) from heightSource
                m_noiseValues[i+5*j] = m_data->heightSource->GetHeight(x,z);

                if(i == 0 && j == 0)
                {
                    m_aabb.x = x;
                    m_aabb.y = m_noiseValues[0] * m_data->quadtree->GetMaximumHeight();
                    m_aabb.z = z;
                    m_aabb.width = 0.1f;
                    m_aabb.depth = 0.1f;
                    m_aabb.height = 0.1;
                }


                m_aabb.ExtendTo(NzVector3f(x,m_noiseValues[i+5*j] * m_data->quadtree->GetMaximumHeight() ,z));
            }

        for(int i(-1) ; i < 6 ; ++i)
            for(int j(-1) ; j < 6 ; ++j)
                m_extraHeightValues.at((i+1)+7*(j+1)) = m_data->heightSource->GetHeight(m_center.x+m_size*(0.25*i-0.5),m_center.y+m_size*(0.25*j-0.5));
    }
}

void NzPatch::ComputeNormals()
{
    if(m_isInitialized)
    {
    //top, right, bottom, left
    NzVector3f v1,v2,v3,v4;
    NzVector3f v12;
    NzVector3f v23;
    NzVector3f v34;
    NzVector3f v41;
    NzVector3f sum;

    unsigned int i0,j0;

    for(unsigned int i(0) ; i < 5 ; ++i)
        for(unsigned int j(0) ; j < 5 ; ++j)
        {
            i0 = i + 1;
            j0 = j + 1;
            //Compute four vectors
            v1.x = m_size * (0.25 * (i0+1) - 0.5);
            v1.y = m_size * (0.25 *  j0    - 0.5);
            v1.z = m_extraHeightValues.at((i0+1) + 7*j0) * m_data->quadtree->GetMaximumHeight();

            v2.x = m_size * (0.25 * i0     - 0.5);
            v2.y = m_size * (0.25 * (j0+1) - 0.5);
            v2.z = m_extraHeightValues.at(i0     + 7*(j0+1)) * m_data->quadtree->GetMaximumHeight();

            v3.x = m_size * (0.25 * (i0-1) - 0.5);
            v3.y = m_size * (0.25 *  j0    - 0.5);
            v3.z = m_extraHeightValues.at((i0-1) + 7*j0) * m_data->quadtree->GetMaximumHeight();

            v4.x = m_size * (0.25 *  i0    - 0.5);
            v4.y = m_size * (0.25 * (j0-1) - 0.5);
            v4.z = m_extraHeightValues.at(i0     + 7*(j0-1)) * m_data->quadtree->GetMaximumHeight();

            v12 = v1.CrossProduct(v2);
            v23 = v2.CrossProduct(v3);
            v34 = v3.CrossProduct(v4);
            v41 = v4.CrossProduct(v1);

            sum = v12 + v23 + v34 + v41;
            sum.Normalize();

            if(sum.DotProduct(NzVector3f(0.f,0.f,1.f)) < 0)//FIX ME : USEFULL
                sum *= -1;

            m_vertexNormals.at(i+5*j) = sum;
        }
    }
}

void NzPatch::ComputeSlope()
{
    if(m_isInitialized)
    {
    float slope[4];
    float dSlope[2];

    float maxSlope = 0.f;

    float h = m_size/20.f;
    float f1,f2,f3,f4;

    for(unsigned int i(0) ; i < 5 ; ++i)
        for(unsigned int j(0) ; j < 5 ; ++j)
        {
            f1 = m_data->heightSource->GetHeight(m_center.x + m_size * (0.25 * i - 0.5)+h, m_center.y + m_size * (0.25 * j - 0.5));
            f2 = m_data->heightSource->GetHeight(m_center.x + m_size * (0.25 * i - 0.5)-h, m_center.y + m_size * (0.25 * j - 0.5));
            f3 = m_data->heightSource->GetHeight(m_center.x + m_size * (0.25 * i - 0.5),   m_center.y + m_size * (0.25 * j - 0.5)+h);
            f4 = m_data->heightSource->GetHeight(m_center.x + m_size * (0.25 * i - 0.5),   m_center.y + m_size * (0.25 * j - 0.5)-h);

            //On calcule les pentes selon x
            slope[0] = std::fabs(f1 - m_noiseValues[i+5*j])/(f1 + m_noiseValues[i+5*j]);
            slope[1] = std::fabs(m_noiseValues[i+5*j] - f2)/(f2 + m_noiseValues[i+5*j]);

            //On calcule les pentes selon y
            slope[2] = std::fabs(f3 - m_noiseValues[i+5*j])/(f3 + m_noiseValues[i+5*j]);
            slope[3] = std::fabs(m_noiseValues[i+5*j] - f4)/(f4 + m_noiseValues[i+5*j]);

            //On calcule le "contraste" de pente selon x
            dSlope[0] = std::fabs(slope[1] - slope[0])/std::fabs(slope[1] + slope[0]);

            //On calcule le "contraste" de pente selon y
            dSlope[1] = std::fabs(slope[3] - slope[2])/std::fabs(slope[3] + slope[2]);

            maxSlope = std::max(dSlope[0],maxSlope);
            maxSlope = std::max(dSlope[1],maxSlope);

        }

        float inv_sensitivity = 10;
        m_slope = std::pow(maxSlope,inv_sensitivity);
    }
}

NzCubef& NzPatch::GetAABB()
{
    return m_aabb;
}

const NzCubef& NzPatch::GetAABB() const
{
    return m_aabb;
}

NzVector2f NzPatch::GetCenter() const
{
    return m_center;
}

float NzPatch::GetSize() const
{
    return m_size;
}

float NzPatch::GetGlobalSlope() const
{
    return m_slope;
}

void NzPatch::Initialize(NzVector2f center, float size, id nodeID, TerrainNodeData* data)
{
    m_id = nodeID;
    m_center = center;
    m_size = size;
    m_data = data;

    m_isUploaded = false;
    m_isInitialized = true;

    m_configuration = 0;

    for(unsigned int i(0) ; i < 25 ; ++i)
        m_noiseValues[i] = 0.f;

    ComputeHeights();

    ComputeNormals();

    ComputeSlope();
}

void NzPatch::Invalidate()
{
    m_isInitialized = false;
}

void NzPatch::SetConfiguration(nzDirection neighborLocation, unsigned int levelDifference, bool autoUpdate)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzPatch::SetConfig : invalid patch called : "<<m_id.lvl<<"|"<<m_id.sx<<"|"<<m_id.sy<<std::endl;
        return;
    }

    if(levelDifference > 1)
    {
        std::cout<<"Difference of level at interface must be < 1, here : "<<levelDifference<<std::endl;
        return;
    }

    unsigned short int newConfiguration = m_configuration;
    //std::cout<<"previous conf "<<m_configuration<<std::endl;
    switch(neighborLocation)
    {
        case LEFT :
            if(levelDifference == 1)
                newConfiguration = newConfiguration | 0x1;
            else
                newConfiguration = newConfiguration & 0xE;
        break;

        case TOP :
            if(levelDifference)
                newConfiguration = newConfiguration | 0x2;
            else
                newConfiguration = newConfiguration & 0xD;
        break;

        case RIGHT :
            if(levelDifference == 1)
                newConfiguration = newConfiguration | 0x4;
            else
                newConfiguration = newConfiguration & 0xB;
        break;

        case BOTTOM :
            if(levelDifference)
                newConfiguration = newConfiguration | 0x8;
            else
                newConfiguration = newConfiguration & 0x7;
        break;
    }

    if(newConfiguration != m_configuration)
    {
        m_configuration = newConfiguration;
        //std::cout<<"Updating patch "<<m_id.lvl<<"|"<<m_id.sx<<"|"<<m_id.sy<<" to conf "<<m_configuration<<" new direction "<<neighborLocation<<std::endl;
        UploadMesh(false);
    }
}

void NzPatch::UploadMesh(bool firstTime)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzPatch::SetConfig : invalid patch called : old node : "<<m_id.lvl<<"|"<<m_id.sx<<"|"<<m_id.sy<<std::endl;
        return;
    }

    //std::cout<<"Uploading patch "<<m_id.lvl<<"|"<<m_id.sx<<"|"<<m_id.sy<<" Update ? "<<firstTime<<std::endl;

    unsigned int index, index2, i2, j2;

    for(int i(0) ; i < 5 ; ++i)
        for(int j(0) ; j < 5 ; ++j)
        {
            index = 5*i+j;
            index2 = index;
            i2 = i;
            j2 = j;

            if(((m_configuration & 0x1) == 0x1) && (index == 5 || index == 15))
            {
                index2 = index + 5;
                i2 = i + 1;
            }
            else if(((m_configuration & 0x4) == 0x4) && (index == 9 || index == 19))
            {
                index2 = index - 5;
                i2 = i - 1;
            }
            else if((m_configuration & 0x2) == 0x2 && (index == 1 || index == 3))
            {
                index2 = index + 1;
                j2 = j + 1;
            }
            else if((m_configuration & 0x8) == 0x8 && (index == 21 || index == 23))
            {
                index2 = index - 1;
                j2 = j - 1;
            }

            //Position
            m_uploadedData.at((index)*6)   = m_center.x + m_size * (0.25 * j2 - 0.5);//X
            m_uploadedData.at((index)*6+1) = m_noiseValues.at(index2) * m_data->quadtree->GetMaximumHeight();//Z
            m_uploadedData.at((index)*6+2) = m_center.y + m_size * (0.25 * i2 - 0.5);//Y
            //Normales
            m_uploadedData.at((index)*6+3) = m_vertexNormals.at(index2).x;
            m_uploadedData.at((index)*6+4) = m_vertexNormals.at(index2).z;
            m_uploadedData.at((index)*6+5) = m_vertexNormals.at(index2).y;
        }

    if(firstTime)
        m_data->dispatcher->SubmitPatch(m_uploadedData,m_id);
    else
        m_data->dispatcher->UpdatePatch(m_uploadedData,m_id);

    m_isUploaded = true;
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

