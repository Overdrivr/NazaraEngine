// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Terrain Renderer module".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/TerrainRenderer/Config.hpp>
#include <Nazara/TerrainRenderer/TerrainChunk.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/TerrainRenderer/Debug.hpp>

NzTerrainChunk::NzTerrainChunk()
{
    //La structure du vertex buffer
    //TODO : Trouver un moyen d'éviter l'initialisation à chaque constructeur
    //TODO : Propager une exception si la déclaration n'est pas réussie
    NzVertexElement m_elements[2];

    m_elements[0].usage = nzElementUsage_Position;
    m_elements[0].offset = 0;
    m_elements[0].type = nzElementType_Float3;

    m_elements[1].usage = nzElementUsage_Normal;
    m_elements[1].offset = 3*sizeof(float);
    m_elements[1].type = nzElementType_Float3;

	if (!m_declaration.Create(m_elements, 2))
		std::cout << "NzTerrainChunk::CreateBuffer : Failed to create vertex declaration" << std::endl;
}

NzTerrainChunk::~NzTerrainChunk()
{
    //dtor
}

bool NzTerrainChunk::AddMesh(const std::array<150,float>& vertexData, const NzBoundingBoxf& meshBoundingBox, nzTerrainNodeID meshIdentifiant)
{
    if(m_vertexBuffersMap.GetTotalFreeSlotsAmount() == 0)
    {
        if(!this->CreateBuffer())
            return false;
    }

    NzVector2i freeSlot = m_vertexBuffersMap.GetFreeSlot();

    if(m_vertexBuffers.at(freeSlot.x)->Fill(vertexData.data(),freeSlot.y * 25,25))
    {
        std::cout<<"NzTerrainChunk::AddMesh : Cannot fill vertex buffer number "<<freeSlot.x<<" at index "<<freeSlot.y * 25<<std::endl;
        return false;
    }
    //TODO : Vérifier si la transaction a bien eu lieu jusqu'au bout ?
    m_vertexBuffersMap.FillFreeSlot(freeSlot,meshIdentifiant);

    return true;
}

bool NzTerrainChunk::UpdateMesh(const std::array<150,float>& vertexData,nzTerrainNodeID meshIdentifiant)
{
    //TOCHECK : 2 recherches dans le sparsebuffer alors qu'une suffirait ?
    if(!m_vertexBuffersMap.DoesKeyExists(meshIdentifiant))
        return false;

    NzVector2i slotToUpdate = m_buffersMap.FindKeyLocation(meshIdentifiant);

    if(!m_buffers.at(slotToUpdate.x)->Fill(vertexData.data(),slotToUpdate.y*25,25))
    {
        std::cout<<"NzTerrainChunk::UpdateMesh : Cannot fill vertex buffer number "<<freeSlot.x<<" at index "<<freeSlot.y * 25<<std::endl;
        return false;
    }

    return true;
}

bool NzTerrainChunk::RemoveMesh(nzTerrainNodeID meshIdentifiant)
{
    //TOCHECK : 2 recherches dans le sparsebuffer alors qu'une suffirait ?
    if(!m_vertexBuffersMap.DoesKeyExists(meshIdentifiant))
        return false;

    NzVector2i slotToRemove = m_buffersMap.FindKeyLocation(meshIdentifiant);

    //TODO : Vérifier si la transaction a bien eu lieu jusqu'au bout ?
    m_vertexBuffersMap.FreeFilledSlot(slotToRemove);

    return true;
}

bool CreateBuffer()
{
    #if NAZARA_TERRAINRENDERER_SAFE
    if(!m_declaration.IsValid())
        return false;
    #endif

	//On ajoute un buffer
	//TOCHECK : static ou dynamic ?
    m_buffers.emplace_back(&m_declaration,1750,nzBufferStorage_Hardware,nzBufferUsage_Static);
    m_buffersMap.AddEmptyBuffer(m_freeSpotsAmount);

    return true;
}
