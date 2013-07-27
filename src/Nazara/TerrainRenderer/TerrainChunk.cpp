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

bool NzTerrainChunk::AddMesh(const std::array<float,150>& vertexData, const NzBoundingBoxf& meshBoundingBox, NzTerrainNodeID meshIdentifiant)
{
    if(m_vertexBuffersMap.GetTotalFreeSlotsAmount() == 0)
    {
        if(!this->CreateBuffer())
            return false;
    }

    NzVector2i freeSlot = m_vertexBuffersMap.GetFreeSlot();

    if(m_vertexBuffers.at(freeSlot.x).Fill(vertexData.data(),freeSlot.y * 25,25))
    {
        std::cout<<"NzTerrainChunk::AddMesh : Cannot fill vertex buffer number "<<freeSlot.x<<" at index "<<freeSlot.y * 25<<std::endl;
        return false;
    }
    //TODO : Vérifier si la transaction a bien eu lieu jusqu'au bout ?
    m_vertexBuffersMap.FillFreeSlot(freeSlot,meshIdentifiant);

    return true;
}

bool NzTerrainChunk::UpdateMesh(const std::array<float,150>& vertexData,NzTerrainNodeID meshIdentifiant)
{
    //TOCHECK : 2 recherches dans le sparsebuffer alors qu'une suffirait ?
    if(!m_vertexBuffersMap.DoesKeyExists(meshIdentifiant))
        return false;

    NzVector2i slotToUpdate = m_vertexBuffersMap.FindKey(meshIdentifiant);

    if(!m_vertexBuffers.at(slotToUpdate.x).Fill(vertexData.data(),slotToUpdate.y*25,25))
    {
        std::cout<<"NzTerrainChunk::UpdateMesh : Cannot fill vertex buffer number "<<slotToUpdate.x<<" at index "<<slotToUpdate.y * 25<<std::endl;
        return false;
    }

    return true;
}

bool NzTerrainChunk::RemoveMesh(NzTerrainNodeID meshIdentifiant)
{
    //TOCHECK : 2 recherches dans le sparsebuffer alors qu'une suffirait ?
    if(!m_vertexBuffersMap.DoesKeyExists(meshIdentifiant))
        return false;

    NzVector2i slotToRemove = m_vertexBuffersMap.FindKey(meshIdentifiant);

    //TODO : Vérifier si la transaction a bien eu lieu jusqu'au bout ?
    //TODO : A besoin de la value pour l'instant, ne peux compiler
    m_vertexBuffersMap.FreeFilledSlot(slotToRemove);

    return true;
}

bool NzTerrainChunk::CreateBuffer()
{
    #if NAZARA_TERRAINRENDERER_SAFE
    if(!m_declaration.IsValid())
        return false;
    #endif

	//On ajoute un buffer
	//TOCHECK : static ou dynamic ?
    m_vertexBuffers.emplace_back(&m_declaration,1750,nzBufferStorage_Hardware,nzBufferUsage_Static);
    m_vertexBuffersMap.AddEmptyBuffer(1750);

    return true;
}
