// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#include "Dispatcher.hpp"
#include <iostream>
#include <cmath>

NzDispatcher::NzDispatcher()
{
    m_isReady = false;


}

NzDispatcher::~NzDispatcher()
{
    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {
        delete m_buffers.at(i);
        m_buffers.at(i) = nullptr;
    }
    m_buffers.clear();
}

void NzDispatcher::DrawAll()
{
    if(m_isReady)
    {

    }
}

unsigned int NzDispatcher::GetFreeBuffersAmount() const
{
    return m_freeBuffers.size();
}

void NzDispatcher::ErasePatch(const id& ID)
{
    if(m_isReady)
    {

    }
}

bool NzDispatcher::Initialize(unsigned int zoneDepth, unsigned int bufferAmount)
{
    ///---- On crée la déclaration de vertices
    m_elements[0].usage = nzElementUsage_Position;
	m_elements[0].offset = 0;
	m_elements[0].type = nzElementType_Float3;

	m_elements[1].usage = nzElementUsage_Normal;
	m_elements[1].offset = 3*sizeof(float);
	m_elements[1].type = nzElementType_Float3;

	if (!m_declaration.Create(m_elements, 2))
	{
		std::cout << "NzDispatcher : Failed to create vertex declaration" << std::endl;
		return false;
	}

	///On crée l'index buffer
        //Un patch a besoin d'un index buffer de 192 points, * 1750 patches = 336000
	m_indexBuffer.reset(new NzIndexBuffer(336000));

    ///-----On crée toutes les zones nécessaires
    m_zoneDepth = zoneDepth;
    m_zonesAmountX = std::pow(2,m_zoneDepth);
    //On crée le nombre de zones demandé
    for(unsigned int i(0) ; i < m_zonesAmountX*m_zonesAmountX ; ++i)
    {
        std::unique_ptr<NzZone> zone(new NzZone(this));
        m_zones.push_back(std::move(zone));
    }

    ///------ On alloue le nombre de buffers demandés si possible
    //FIX ME : Que se passe t'il si on demande trop de mémoire vidéo ?
    for(unsigned int i(0) ; i < bufferAmount ; ++i)
    {
        NzVertexBuffer* buffer = new NzVertexBuffer(&m_declaration,262500, nzBufferStorage_Hardware, nzBufferUsage_Static);
        m_buffers.push_back(buffer);
        //On met tous ces buffers dans la file de buffers libres
        m_freeBuffers.push(buffer);
        //std::cout<<"buffer capacity "<<m_freeBuffers.back()->GetVertexCount()<<"|"<<i<<std::endl;
    }
    m_isReady = true;
    return true;
}

NzVertexBuffer* NzDispatcher::QueryFreeBuffer()
{
    NzVertexBuffer* buffer = nullptr;

    if(m_freeBuffers.size() > 0 && m_isReady)
    {
        buffer = m_freeBuffers.front();
        m_freeBuffers.pop();
    }

    return buffer;
}

void NzDispatcher::ReturnBuffer(NzVertexBuffer* buffer)
{
    m_freeBuffers.push(buffer);
}

bool NzDispatcher::SubmitPatch(const std::array<float,150>& subBuffer, const id& ID)
{
    if(!m_isReady)
        return false;

    //On récupère la zone devant accueillir le patch
    id temp;
    temp.sx = ID.sx*m_zonesAmountX/std::pow(2,ID.lvl);
    temp.sy = ID.sy*m_zonesAmountX/std::pow(2,ID.lvl);

    if(temp.sx < m_zonesAmountX && temp.sy < m_zonesAmountX)
    {
        std::cout<<"submitting patch to zone "<<temp.sx<<" | "<<temp.sy<<std::endl;
        m_zones.at(temp.sx + m_zonesAmountX*temp.sy)->AddPatch(subBuffer,ID);
        return true;
    }
    else
    {
        std::cout<<"Submitting patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
        return false;
    }

}

bool NzDispatcher::UpdatePatch(const std::array<float,150>& subBuffer, const id& ID)
{
    if(m_isReady)
    {
        //On récupère la zone devant accueillir le patch
        id temp;
        temp.sx = ID.sx*m_zonesAmountX/std::pow(2,ID.lvl);
        temp.sy = ID.sy*m_zonesAmountX/std::pow(2,ID.lvl);

        if(temp.sx < m_zonesAmountX && temp.sy < m_zonesAmountX)
        {
            std::cout<<"submitting patch to zone "<<temp.sx<<" | "<<temp.sy<<std::endl;
            m_zones.at(temp.sx + m_zonesAmountX*temp.sy)->UpdatePatch(subBuffer,ID);
            return true;
        }
        else
        {
            std::cout<<"Submitting patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
            return false;
        }
    }
    else
        return false;
}

void NzDispatcher::UpdateViewFrustumCulling()
{
    if(m_isReady)
    {

    }
}
