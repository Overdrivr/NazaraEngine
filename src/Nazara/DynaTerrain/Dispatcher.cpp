// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#include "Dispatcher.hpp"
#include <iostream>
#include <cmath>

NzDispatcher::NzDispatcher()
{
	m_elements[0].usage = nzElementUsage_Position;
	m_elements[0].offset = 0;
	m_elements[0].type = nzElementType_Float3;

	m_elements[1].usage = nzElementUsage_Normal;
	m_elements[1].offset = 3*sizeof(float);
	m_elements[1].type = nzElementType_Float3;

/*	if (!declaration.Create(elements, 2))
	{
		std::cout << "NzDispatcher : Failed to create vertex declaration" << std::endl;
	}*/
}

NzDispatcher::~NzDispatcher()
{
    for(int i(0) ; i < m_zones.size() ; ++i)
    {
        delete m_zones.at(i);
        m_zones.at(i) = nullptr;
    }
/*
    for(int i(0) ; i < m_buffers.size() ; ++i)
    {
        delete m_buffers.at(i);
        m_buffers.at(i) = nullptr;
    }*/
}

void NzDispatcher::DrawAll()
{

}

unsigned int NzDispatcher::GetFreeBuffersAmount() const
{
    return m_freeBuffersAmount;
}

void NzDispatcher::ErasePatch(const id& ID)
{

}

void NzDispatcher::Initialize(unsigned int zoneDepth, unsigned int bufferAmount)
{
    m_zoneDepth = zoneDepth;
    //On crée le nombre de zones demandé
    for(int i(0) ; i < (1<<zoneDepth) ; ++i)
        for(int j(0) ; j < (1<<zoneDepth) ; ++j)
        {
            NzZone* zone = new NzZone;
            m_zones.push_back(zone);
        }
    int nbZones = (1<<zoneDepth)*(1<<zoneDepth);
    std::cout<< nbZones<<" created zones"<<std::endl;

    m_totalBuffersAmount = bufferAmount;
    //On alloue le nombre de buffers demandés si possible
    /*for(int i(0) ; i < m_bufferAmount ; ++i)
    {
        NzVertexBuffer* buffer = new NzVertexBuffer;
        m_buffers.push_back(buffer);
    }*/

    //On met tous ces buffers dans la file de buffers libres
    //
    m_freeBuffersAmount = m_totalBuffersAmount;
}

bool NzDispatcher::QueryFreeBuffer(NzVertexBuffer* buffer)
{
    if(m_freeBuffersAmount > 0)
    {
        buffer = m_freeBuffers.front();
        m_freeBuffers.pop();
        m_freeBuffersAmount--;

        return true;
    }
    else
        return false;
}

bool NzDispatcher::SubmitPatch(const std::array<float,150>& subBuffer, const id& ID)
{
    //On récupère la zone devant accueillir le patch
    id temp;
    temp.sx = ID.sx*(1<<m_zoneDepth)/(1<<ID.lvl);
    temp.sy = ID.sy*(1<<m_zoneDepth)/(1<<ID.lvl);

    if(temp.sx < (1<<m_zoneDepth) && temp.sy < (1<<m_zoneDepth))
    {
        m_zones.at(temp.sx + (1<<m_zoneDepth)*temp.sy)->AddPatch(subBuffer,ID);
        return true;
    }
    else
    {
        std::cout<<"Submitting patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" outside supported area :"<<std::pow(2,m_zoneDepth)<<std::endl;
        return false;
    }

}


bool NzDispatcher::UpdatePatch(const std::array<float,150>& subBuffer, const id& ID)
{
    //On récupère la zone devant accueillir le patch
    id temp;
    temp.sx = ID.sx*(1<<m_zoneDepth)/(1<<ID.lvl);
    temp.sy = ID.sy*(1<<m_zoneDepth)/(1<<ID.lvl);

    if(temp.sx < (1<<m_zoneDepth) && temp.sy < (1<<m_zoneDepth))
    {
        m_zones.at(temp.sx + (1<<m_zoneDepth)*temp.sy)->UpdatePatch(subBuffer,ID);
    }
    else
    {
        std::cout<<"Submitting patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" outside supported area :"<<std::pow(2,m_zoneDepth)<<std::endl;
        return false;
    }

}


void NzDispatcher::ViewFrustumCulling()
{

}
