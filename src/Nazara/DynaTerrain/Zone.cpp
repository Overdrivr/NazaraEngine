// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#include "Zone.hpp"
#include <iostream>
#include "Dispatcher.hpp"

NzZone::NzZone(NzDispatcher* dispatcher)
{
    m_dispatcher = dispatcher;
}

NzZone::~NzZone()
{
    //dtor
}

void NzZone::AddBuffer(NzVertexBuffer* buffer)
{
    m_buffers.push_back(buffer);
    std::cout<<"Adding buffer "<<m_buffers.size()-1<<std::endl;
    //Un buffer contient 1750 emplacements
    for(int i(0) ; i < 1750 ; ++i)
    {
        nzBufferLocation temp;
        temp.buffer = m_buffers.size() - 1;
        temp.index = i*25;
        m_freeSpotsIndex.push_back(temp);
    }
    std::cout<<"Free spots : "<<m_freeSpotsIndex.size()<<std::endl;
}

void NzZone::AddPatch(const std::array<float,150>& vertices, const id& ID)
{
    std::cout<<"Adding patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<std::endl;

    //Si la zone n'a pas de buffers de libre, elle en demande un au dispatcher
    if(m_freeSpotsIndex.size() == 0)
    {
        NzVertexBuffer* temp = m_dispatcher->QueryFreeBuffer();
        if(temp != nullptr)
        {
            //On a bien reçu un buffer, on peut poursuivre
            this->AddBuffer(temp);
        }
        else //Si le dispatcher ne peut en fournir aucun, l'opération est temporairement abandonnée et le patch est sauvegardé dans une file
        {
            for(int i(0) ; i < 150 ; ++i)
            {
                m_unbufferedPatches.push(vertices.at(i));
                m_unbufferedPatchesIndex.push(ID);
            }
            return;
        }
    }


    //La zone a desormais des slots de libres, on enregistre les vertices
    nzBufferLocation location = m_freeSpotsIndex.front();

    if(!m_buffers.at(location.buffer)->Fill(vertices.data(),location.index,25))
    {
        std::cout<<"Cannot fill buffer"<<std::endl;
        return;
    }

    m_patchesIndex.insert(std::pair<id,nzBufferLocation>(ID,location));
    m_freeSpotsIndex.pop_front();
}

void NzZone::DrawBuffers()
{

}

unsigned int NzZone::GetFreeBuffersAmount()
{
    //FIX ME
    return 0;
}

unsigned int NzZone::GetFreeSubBuffersAmount()
{
    return m_freeSpotsIndex.size();
}

bool NzZone::RemoveFreeBuffer(NzVertexBuffer* buffer)
{
    return false;
}

bool NzZone::RemovePatch(const id& ID)
{
    return false;
}

bool NzZone::UpdatePatch(const std::array<float,150>& vertices, const id& ID)
{

}
