// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <cmath>

NzDispatcher::NzDispatcher()
{
    m_isReady = false;

    //On construit l'index buffer
    //taille totale : 1750 * 96 = 168000
    //taille atomique : 96
    //32 triangles

    unsigned int rowIndex[24];
    for(int i(0) ; i < 4 ; ++i)
    {
        rowIndex[i*6] = i;
        rowIndex[i*6+1] = i + 1;
        rowIndex[i*6+2] = i + 6;
        rowIndex[i*6+3] = i;
        rowIndex[i*6+4] = i + 5;
        rowIndex[i*6+5] = i + 6;
    }
    unsigned int indexes[96];


    for(unsigned int i(0) ; i < 4 ; ++i)
        for(unsigned int j(0) ; j < 24 ; ++j)
        {
            indexes[i*24+j] = rowIndex[j] + i*5;
        }

    //L'index entier
    unsigned int allIndexes[168000];

    for(int i(0) ; i < 1750 ; ++i)
    {
        for(int j(0) ; j < 96 ; ++j)
        {
            allIndexes[i*96+j] = indexes[j] + 25*i;
        }
    }

	m_indexBuffer = new NzIndexBuffer(168000, true, nzBufferStorage_Hardware);
	if (!m_indexBuffer->Fill(allIndexes, 0, 168000)) // FIX ME : Que faire en cas d'échec
	{
		std::cout << "Failed to fill indexbuffer" << std::endl;
	}

}

NzDispatcher::~NzDispatcher()
{
    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {
        delete m_buffers.at(i);
        m_buffers.at(i) = nullptr;
    }
    m_buffers.clear();

    delete m_indexBuffer;
}

void NzDispatcher::DrawAll(bool viewFrustumCullingEnabled)
{
    if(m_isReady)
    {
        NzRenderer::SetIndexBuffer(m_indexBuffer);

        for(unsigned int i(0) ; i < m_zones.size() ; ++i)
        {
            m_zones.at(i)->DrawBuffers();
        }
    }
}

unsigned int NzDispatcher::GetFreeBuffersAmount() const
{
    return m_freeBuffers.size();
}

bool NzDispatcher::RemovePatch(const id& ID)
{
    if(m_isReady)
    {
        //On récupère la zone devant accueillir le patch
        id temp;
        temp.sx = ID.sx*m_zonesAmountX/std::pow(2,ID.lvl);
        temp.sy = ID.sy*m_zonesAmountX/std::pow(2,ID.lvl);

        if(temp.sx < m_zonesAmountX && temp.sy < m_zonesAmountX)
        {
            //std::cout<<"submitting patch to zone "<<temp.sx<<" | "<<temp.sy<<std::endl;
            //std::cout<<"Trying Removing patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" in Zone "<<temp.lvl<<"|"<<temp.sx<<"|"<<temp.sy<<" with buf"<<std::endl;
            m_zones.at(temp.sx + m_zonesAmountX*temp.sy)->RemovePatch(ID);
            return true;
        }
        else
        {
            std::cout<<"Removing patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
            return false;
        }
    }
    std::cout<<"Impossible to remove patch"<<std::endl;
    return false;
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

    ///-----On crée toutes les zones nécessaires
    m_zoneDepth = zoneDepth;
    m_zonesAmountX = std::pow(2,m_zoneDepth);
    //On crée le nombre de zones demandé
    for(unsigned int i(0) ; i < m_zonesAmountX*m_zonesAmountX ; ++i)
    {
        std::unique_ptr<NzZone> zone(new NzZone(this));
        m_zones.push_back(std::move(zone));
    }
/*
    if(bufferAmount < m_zonesAmountX*m_zonesAmountX*1.5)
        bufferAmount = static_cast<unsigned int>(m_zonesAmountX*m_zonesAmountX*1.5f);

    if(bufferAmount > 50)
    {
        //On ne souhaite pas allouer plus de 50 Mio de mémoire vidéo au terrain
        //FIX ME : régler la taille des buffers en fonction du nombre de zones
        bufferAmount = 50;
    }

    std::cout<<"Real buffer amount : "<<bufferAmount<<std::endl;

    ///------ On alloue le nombre de buffers demandés si possible
    //FIX ME : Que se passe t'il si on demande trop de mémoire vidéo ?
    for(unsigned int i(0) ; i < bufferAmount ; ++i)
    {
        NzVertexBuffer* buffer = new NzVertexBuffer(&m_declaration,262500, nzBufferStorage_Hardware, nzBufferUsage_Static);
        m_buffers.push_back(buffer);
        //On met tous ces buffers dans la file de buffers libres
        m_freeBuffers.push(buffer);
        //std::cout<<"buffer capacity "<<m_freeBuffers.back()->GetVertexCount()<<"|"<<i<<std::endl;
    }*/
    m_isReady = true;
    return true;
}

NzVertexBuffer* NzDispatcher::QueryFreeBuffer()
{
    NzVertexBuffer* buffer = nullptr;

    if(m_freeBuffers.size() > 0)
    {
        buffer = m_freeBuffers.front();
        m_freeBuffers.pop();
    }
    else
    {
        buffer = new NzVertexBuffer(&m_declaration,262500, nzBufferStorage_Hardware, nzBufferUsage_Static);
        m_buffers.push_back(buffer);
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
        //std::cout<<"Submitting patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" in Zone "<<temp.lvl<<"|"<<temp.sx<<"|"<<temp.sy<<std::endl;
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
            //std::cout<<"submitting patch to zone "<<temp.sx<<" | "<<temp.sy<<std::endl;
            m_zones.at(temp.sx + m_zonesAmountX*temp.sy)->UpdatePatch(subBuffer,ID);
            return true;
        }
        else
        {
            std::cout<<"Updating patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
            return false;
        }
    }
    else
        return false;
}
