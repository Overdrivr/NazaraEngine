// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/Patch.hpp>
#include <Nazara/DynaTerrain/TerrainMasterNode.hpp>
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
    for(int j(0) ; j < 7 ; ++j)
    {
        m_vertexPositions.at(0+7*j) = m_data->quadtree->GetVertexPosition(m_id,-1,j-1);
        m_vertexPositions.at(1+7*j) = m_data->quadtree->GetVertexPosition(m_id,0,j-1);
        m_vertexPositions.at(2+7*j) = m_data->quadtree->GetVertexPosition(m_id,1,j-1);
        m_vertexPositions.at(3+7*j) = m_data->quadtree->GetVertexPosition(m_id,2,j-1);
        m_vertexPositions.at(4+7*j) = m_data->quadtree->GetVertexPosition(m_id,3,j-1);
        m_vertexPositions.at(5+7*j) = m_data->quadtree->GetVertexPosition(m_id,4,j-1);
        m_vertexPositions.at(6+7*j) = m_data->quadtree->GetVertexPosition(m_id,5,j-1);
    }

    m_aabb.x = m_vertexPositions.at(8).x;
    m_aabb.y = m_vertexPositions.at(8).y;
    m_aabb.z = m_vertexPositions.at(8).z;
    m_aabb.width = 0.1f;
    m_aabb.depth = 0.1f;
    m_aabb.height = 0.1f;

    for(int j(1) ; j < 6 ; ++j)
    {
        m_aabb.ExtendTo(m_vertexPositions.at(1+7*j));
        m_aabb.ExtendTo(m_vertexPositions.at(2+7*j));
        m_aabb.ExtendTo(m_vertexPositions.at(3+7*j));
        m_aabb.ExtendTo(m_vertexPositions.at(4+7*j));
        m_aabb.ExtendTo(m_vertexPositions.at(5+7*j));
    }
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

    for(unsigned int j(0) ; j < 5 ; ++j)
        for(unsigned int i(0) ; i < 5 ; ++i)
        {
            i0 = i + 1;
            j0 = j + 1;
            //Compute four vectors
            v1 = m_vertexPositions.at(i0+1 + 7*j0);

            v2 = m_vertexPositions.at(i0   + 7*(j0+1));

            v3 = m_vertexPositions.at(i0-1 + 7*j0);

            v4 = m_vertexPositions.at(i0+1 + 7*(j0-1));

            v12 = v1.CrossProduct(v2);
            v23 = v2.CrossProduct(v3);
            v34 = v3.CrossProduct(v4);
            v41 = v4.CrossProduct(v1);

            sum = v12 + v23 + v34 + v41;
            sum.Normalize();

            m_vertexNormals.at(i+5*j) = sum;
        }
}

void NzPatch::ComputeSlope()
{
    float slope[25];
    NzVector3f upVector(0.f,1.f,0.f);

    float maxSlope = -10000.f;
    float minSlope = 10000.f;

    for(unsigned int j(0) ; j < 5 ; ++j)
        for(unsigned int i(0) ; i < 5 ; ++i)
        {
            slope[i+5*j] = m_vertexNormals.at(i+5*j).DotProduct(upVector);
            minSlope = std::min(std::fabs(slope[i+5*j]),minSlope);
            maxSlope = std::max(std::fabs(slope[i+5*j]),maxSlope);
        }

        //On calcule le contraste absolu entre la pente la plus forte et la plus faible
        m_slope = (maxSlope - minSlope)/(maxSlope + minSlope);
        float inv_sensitivity = 2;
        m_slope = std::pow(m_slope,inv_sensitivity);
}

NzCubef& NzPatch::GetAABB()
{
    return m_aabb;
}

const NzCubef& NzPatch::GetAABB() const
{
    return m_aabb;
}

float NzPatch::GetGlobalSlope() const
{
    return m_slope;
}

void NzPatch::Initialize(NzTerrainNodeID nodeID, TerrainNodeData* data)
{
    m_id = nodeID;
    m_data = data;

    m_isUploaded = false;
    m_isInitialized = true;

    m_configuration = 0;

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
        std::cout<<"NzPatch::SetConfig : invalid patch called : "<<m_id.depth<<"|"<<m_id.locx<<"|"<<m_id.locy<<std::endl;
        return;
    }

    if(levelDifference > 1)
    {
        std::cout<<"Difference of level at interface must be < 1, here : "<<levelDifference<<std::endl;
        return;
    }

    unsigned short int newConfiguration = m_configuration;

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
        UploadMesh(false);
    }
}

void NzPatch::UploadMesh(bool firstTime)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzPatch::SetConfig : invalid patch called : old node : "<<m_id.depth<<"|"<<m_id.locx<<"|"<<m_id.locy<<std::endl;
        return;
    }

    unsigned int index, index2, index3, i2, j2;

    for(int j(0) ; j < 5 ; ++j)
        for(int i(0) ; i < 5 ; ++i)
        {
            index = i+5*j;

            i2 = i;
            j2 = j;

            if(((m_configuration & 0x1) == 0x1) && (index == 5 || index == 15))
                j2 += 1;
            else if(((m_configuration & 0x4) == 0x4) && (index == 9 || index == 19))
                j2 -= 1;
            else if((m_configuration & 0x2) == 0x2 && (index == 1 || index == 3))
                i2 += 1;
            else if((m_configuration & 0x8) == 0x8 && (index == 21 || index == 23))
                i2 -= 1;

            index2 = (i2+1)+7*(j2+1);
            index3 = i2 + 5*j2;

            //Position
            m_uploadedData.at((index)*6)   = m_vertexPositions.at(index2).x;
            m_uploadedData.at((index)*6+1) = m_vertexPositions.at(index2).y;
            m_uploadedData.at((index)*6+2) = m_vertexPositions.at(index2).z;
            //Normales
            m_uploadedData.at((index)*6+3) = m_vertexNormals.at(index3).x;
            m_uploadedData.at((index)*6+4) = m_vertexNormals.at(index3).y;
            m_uploadedData.at((index)*6+5) = m_vertexNormals.at(index3).z;
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
        m_data->dispatcher->RemovePatch(m_id);
        m_isUploaded = false;
    }
}

