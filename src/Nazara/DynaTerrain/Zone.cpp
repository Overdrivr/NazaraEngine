// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

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
    //Un buffer contient 1750 emplacements

    /*
    for(int i(0) ; i < 1750 ; ++i)
    {
        nzBufferLocation temp;
        temp.buffer = m_buffers.size() - 1;
        temp.index = i*25;
        //On rajoute l'index de l'emplacement à la file des index libres
        m_freeSpotsIndex.push_back(temp);
    }

    xid ID;
    ID.dummy = true;
    ID.freeAdjacentConsecutiveSlots = 1750;
    std::list<xid> temp;
    temp.push_back(ID);
    m_sparseBuffers.push_back(temp);*/

    //NzSparseBuffer<id> buf(1750);
    //m_buffersImage.push_back(buf);
    std::cout<<"Add Buffer"<<std::endl;
}

void NzZone::AddPatch(const std::array<float,150>& vertices, const id& ID)
{
    //std::cout<<"Adding patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<std::endl;

    //Si la zone n'a pas de slots de libre, elle demande un buffer supplémentaire au dispatcher
    if(m_buffersImage.GetTotalFreeSlotsAmount() == 0)
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

    /*nzBufferLocation location = m_freeSpotsIndex.front();
    m_freeSpotsIndex.pop_front();

    std::cout<<"Free spot "<<m_freeSpotsIndex.size()<<" | "<<location.buffer<<" | "<<location.index<<std::endl;*/



    nzBufferLocation location = m_buffersImage.InsertValue(ID);

    if(location.buffer < 0)
        return;

    //La zone a des slots de libres, on remplit le buffer avec les vertices
    if(!m_buffers.at(location.buffer)->Fill(vertices.data(),location.index,25))
    {
        std::cout<<"Cannot fill buffer"<<std::endl;
        return;
    }
    /*
    //On stocke les informations dans la structure 1 (recherche de patch)
    m_patchesIndex.insert(std::pair<id,nzBufferLocation>(ID,location));
    //On stocke les informations dans la structure 2 (management mémoire)
    this->InsertSparseBufferValue(location,ID);*/


}

void NzZone::DrawBuffers()
{

}

unsigned int NzZone::GetFreeBuffersAmount()
{
    return m_buffersImage.GetFreeBuffersAmount();
}

unsigned int NzZone::GetFreeSlotsAmount()
{
    return m_buffersImage.GetTotalFreeSlotsAmount();
}

void NzZone::Optimize(int amount)
{
    //Here we try to reduce fragmentation in the vertex buffers
        //the image buffer indicates us where to move the raw data

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
