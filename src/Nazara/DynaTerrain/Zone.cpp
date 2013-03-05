// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/Zone.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

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
    m_buffersMap.AddEmptyBuffer(1750);

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

    //std::cout<<"Added Buffer"<<std::endl;
}

void NzZone::AddPatch(const std::array<float,150>& vertices, const NzTerrainNodeID& ID)
{
    //std::cout<<"Adding patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<std::endl;

    //Si la zone n'a pas de slots de libre, elle demande un buffer supplémentaire au dispatcher
    if(m_buffersMap.GetTotalFreeSlotsAmount() == 0)
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

    NzVector2i location = m_buffersMap.InsertValueKey(ID);

    if(location.x < 0)
        return;

    //La zone a des slots de libres, on remplit le buffer avec les vertices
    if(!m_buffers.at(location.x)->Fill(vertices.data(),location.y*25,25))
    {
        std::cout<<"Cannot fill buffer"<<std::endl;
        return;
    }
}

void NzZone::DrawBuffers() const
{

    //For each buffer
    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {

        //we recover consecutive patches amount
        std::list<NzBatch>::const_iterator it = m_buffersMap.at(i).GetFilledBatches().cbegin();
        for(it = m_buffersMap.at(i).GetFilledBatches().cbegin() ; it != m_buffersMap.at(i).GetFilledBatches().cend() ; ++it)
        {
            //We render each patch batch in a single call to reduce draw calls
            NzRenderer::SetVertexBuffer(m_buffers.at(i));

            // On fait le rendu
                //(*it).x -> firstIndex;
                //(*it).y -> vertexCount;
            //Pour dessiner 1 patch (25 vertex) il nous faut 96 index
            NzRenderer::DrawIndexedPrimitives(nzPrimitiveType_TriangleList, (*it).Start()*96, (*it).Count()*96);
            //NzRenderer::DrawPrimitives(nzPrimitiveType_TriangleFan, (*it).x*25, (*it).y*25);
        }
    }
}

unsigned int NzZone::GetFreeBuffersAmount()
{
    return m_buffersMap.GetFreeBuffersAmount();
}

unsigned int NzZone::GetFreeSlotsAmount()
{
    return m_buffersMap.GetTotalFreeSlotsAmount();
}

void NzZone::Optimize(int amount)
{
    //Here we try to reduce fragmentation in the vertex buffers
        //the image buffer indicates us where to move the raw data

    for(int i(0) ; i < amount ; ++i)
    {
        //We recover the initial and final position
        NzVector4i move_data = m_buffersMap.ReduceFragmentation();

        //If there is no fragmentation
        if((move_data.x || move_data.z) < 0)
            return;

        //We reduce fragmentation by moving 1 patch from one position to an other
            //We copy the original data
        //m_buffers.
            //To the new location
        //m_buffers.
    }
}

bool NzZone::RemoveFreeBuffer(NzVertexBuffer* buffer)
{
    return false;
}

bool NzZone::RemovePatch(const NzTerrainNodeID& ID)
{
    NzVector2i location = m_buffersMap.RemoveValueKey(ID);

    if(location.x < 0 || location.y < 0)
        return false;
/*
    float vertices[150] = {0};

    if(!m_buffers.at(location.x)->Fill(vertices,location.y*25,25))
    {
        std::cout<<"Cannot fill buffer"<<std::endl;
        return false;
    }*/
    return true;
}

bool NzZone::UpdatePatch(const std::array<float,150>& vertices, const NzTerrainNodeID& ID)
{
    NzVector2i location = m_buffersMap.FindKeyLocation(ID);

    //Si l'emplacement n'a pas été retrouvé, on abandonne
    if(location.x < 0 || location.y < 0)
    {
        std::cout<<"Cannot update patch...Location not found"<<std::endl;
        return false;
    }


    if(!m_buffers.at(location.x)->Fill(vertices.data(),location.y*25,25))
    {
        std::cout<<"Cannot fill buffer"<<std::endl;
        return false;
    }

    return true;
}
