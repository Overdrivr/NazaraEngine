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
    //Un buffer contient 1750 emplacements
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
    m_sparseBuffers.push_back(temp);
    std::cout<<"Add Buffer"<<std::endl;
}

void NzZone::AddPatch(const std::array<float,150>& vertices, const id& ID)
{
    //std::cout<<"Adding patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<std::endl;

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

    nzBufferLocation location = m_freeSpotsIndex.front();
    m_freeSpotsIndex.pop_front();

    std::cout<<"Free spot "<<m_freeSpotsIndex.size()<<" | "<<location.buffer<<" | "<<location.index<<std::endl;

    //La zone a desormais des slots de libres, on remplit le buffer avec les vertices
    if(!m_buffers.at(location.buffer)->Fill(vertices.data(),location.index,25))
    {
        std::cout<<"Cannot fill buffer"<<std::endl;
        return;
    }
    //On stocke les informations dans la structure 1 (recherche de patch)
    m_patchesIndex.insert(std::pair<id,nzBufferLocation>(ID,location));
    //On stocke les informations dans la structure 2 (management mémoire)
    this->InsertSparseBufferValue(location,ID);


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

void NzZone::Optimize(int amount)
{
    //Here we try to reduce fragmentation in the vertex buffers
        //we use the sparse buffer (image of the vertex buffer)

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
//////////////////////////////////////////////////////////////////////////////////////////
bool NzZone::InsertSparseBufferValue(const nzBufferLocation& loc, const id& ID)
{
    //! Les indices entre le sparsebuffer et le vertexbuffer sont décalés de +1 dans le sparsebuffer
    std::list<xid>::iterator it = m_sparseBuffers.at(loc.buffer).begin();

    for(int i(0) ; i < loc.index ; ++i)
        it++;

    //Si la case précédente indique que la case suivante est libre
    if((*it).freeAdjacentConsecutiveSlots > 0)
    {
        //La case précédente transmet son nombre de cases adjacentes libres (- 1) à sa nouvelle case suivante
        unsigned int tempFreeSlots = (*it).freeAdjacentConsecutiveSlots;

        //On peut insérer une case à l'emplacement suivant
        it++;
        xid value;
        value.sx = ID.sx;
        value.sy = ID.sy;
        value.lvl = ID.lvl;
        value.freeAdjacentConsecutiveSlots = tempFreeSlots - 1;
        std::cout<<"val "<<value.freeAdjacentConsecutiveSlots<<std::endl;
        m_sparseBuffers.at(loc.buffer).insert(it,value);

        return true;
    }
    else
        return false;

}

bool NzZone::RemoveSparseBufferValue(const nzBufferLocation& loc)
{
    return false;
}
