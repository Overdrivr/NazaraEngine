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
    //TODO : A deplacer dans module, fournir une méthode statique pour récupérer la déclaration
    NzVertexElement m_elements[2];

    m_elements[0].usage = nzElementUsage_Position;
    m_elements[0].offset = 0;
    m_elements[0].type = nzElementType_Float3;

    m_elements[1].usage = nzElementUsage_Normal;
    m_elements[1].offset = 3*sizeof(float);
    m_elements[1].type = nzElementType_Float3;

	if (!m_declaration.Create(m_elements, 2))
		std::cout << "NzTerrainChunk::CreateBuffer : Failed to create vertex declaration" << std::endl;

    m_freeSlotsAmount = 0;
}

NzTerrainChunk::~NzTerrainChunk()
{
    //dtor
}

bool NzTerrainChunk::AddMesh(const std::array<float,150>& vertexData, const NzBoundingBoxf& meshBoundingBox, NzTerrainNodeID meshIdentifiant)
{
    int buffer = 0;
    int index = 0;

    // Si pas de place pour un mesh supplémentaire
    if(m_freeSlotsAmount == 0)
    {
        //Création d'un buffer supplémentaire
        if(!this->CreateBuffer())
            return false;
        else
        {
            //Le buffer a bien été crée, le slot disponible est l'index 0 du dernier buffer
            buffer = m_vertexBuffers.size() - 1;
        }

    }
    else
    {
        //Sinon on cherche le premier buffer avec un slot disponible
        for(int i(0) ; i < m_vertexBuffers.size() ; ++i)
        {
            if(m_vertexBuffersMap.at(i).GetFreeSlotsAmount() > 0)
            {
                buffer = i;
                index = m_vertexBuffersMap.at(i).GetFreeSlot();
                break;
            }
        }
    }

    if(!m_vertexBuffers.at(buffer).Fill(vertexData.data(),index * 25,25))
    {
        std::cout<<"NzTerrainChunk::AddMesh : Cannot fill vertex buffer number "<<buffer<<" at index "<<index * 25<<std::endl;
        return false;
    }

    --m_freeSlotsAmount;
    m_vertexBuffersMap.at(buffer).FillFreeSlot(index,meshIdentifiant);

    return true;
}

//TO UPDATE
bool NzTerrainChunk::UpdateMesh(const std::array<float,150>& vertexData,NzTerrainNodeID meshIdentifiant)
{
    int buffer = 0;
    int index = 0;

    // Recherche bête dans chaque buffer l'un après l'autre
    for(int i(0) ; i < m_vertexBuffers.size() ; ++i)
    {
        index = m_vertexBuffersMap.at(i).FindValue(meshIdentifiant);

        //Index valide, on a trouvé l'emplacement
        if(index > -1)
        {
            buffer = i;
            break;
        }
    }

    if(index < 0)
        return false;

    if(!m_vertexBuffers.at(buffer).Fill(vertexData.data(),index * 25,25))
    {
        std::cout<<"NzTerrainChunk::UpdateMesh : Cannot fill vertex buffer number "<<buffer<<" at index "<<index * 25<<std::endl;
        return false;
    }

    return true;
}

//TO UPDATE
bool NzTerrainChunk::RemoveMesh(NzTerrainNodeID meshIdentifiant)
{
    int buffer = 0;
    int index = 0;

    // Recherche bête dans chaque buffer l'un après l'autre
    for(int i(0) ; i < m_vertexBuffers.size() ; ++i)
    {
        index = m_vertexBuffersMap.at(i).FindValue(meshIdentifiant);

        //Index valide, on a trouvé l'emplacement
        if(index > -1)
        {
            buffer = i;
            break;
        }
    }

    if(index < 0)
        return false;

    m_vertexBuffersMap.at(buffer).FreeFilledSlot(index);
    ++m_freeSlotsAmount;

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
    m_vertexBuffersMap.emplace_back(NzIntervalBuffer<NzTerrainNodeID>(1750));
    m_freeSlotsAmount += 1750;

    return true;
}
