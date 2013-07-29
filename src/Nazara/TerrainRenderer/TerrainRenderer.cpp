// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine - Terrain renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/TerrainRenderer/TerrainRenderer.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/TerrainRenderer/Config.hpp>
#include <Nazara/TerrainRenderer/Debug.hpp>

namespace
{
    //La déclaration de structure de tous les vertex buffer
    NzVertexDeclaration m_declaration;
}

void NzTerrainRenderer::DrawTerrainChunk(const NzTerrainChunk& chunk)
{
    // Pour itérer sur les vertex buffers
    auto itBuffers = chunk.m_vertexBuffers.begin();
    unsigned int i = 0;

    for( ; itBuffers != chunk.m_vertexBuffers.end() ; ++itBuffers)
    {
        // Pour itérer sur les lots de maillage dans un même vertexBuffer
        auto itBatches = chunk.m_vertexBuffersMap.at(i).GetFilledIntervals().cbegin();

        // On envoie le vertexBuffer entier au renderer Nazara
        NzRenderer::SetVertexBuffer(&(*itBuffers));

        // On itère sur l'ensemble des lots d'un même buffer
        for(; itBatches != chunk.m_vertexBuffersMap.at(i).GetFilledIntervals().cend() ; ++itBatches)
        {
            // On fait le rendu
                //(*it).x -> firstIndex;
                //(*it).y -> vertexCount;
            //Pour dessiner 1 patch (25 vertex) il nous faut 96 index
            NzRenderer::DrawIndexedPrimitives(nzPrimitiveType_TriangleList, (*itBatches).Start()*96, (*itBatches).Count()*96);
        }
        ++i;
    }
}

const NzVertexDeclaration& NzTerrainRenderer::GetVertexDeclaration()
{
    return m_declaration;
}

bool NzTerrainRenderer::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	// Initialisation des dépendances
	if (!NzRenderer::Initialize())
	{
		NazaraError("Failed to initialize renderer module");
		return false;
	}

	// Initialisation du module
	 //La structure du vertex buffer
    NzVertexElement m_elements[2];

    m_elements[0].usage = nzElementUsage_Position;
    m_elements[0].offset = 0;
    m_elements[0].type = nzElementType_Float3;

    m_elements[1].usage = nzElementUsage_Normal;
    m_elements[1].offset = 3*sizeof(float);
    m_elements[1].type = nzElementType_Float3;

	if (!m_declaration.Create(m_elements, 2))
	{
	    NazaraError("Failed to initialize terrain renderer module : Failed to create vertex declaration");
	    return false;
	}

	NazaraNotice("Initialized: TerrainRenderer module");

	return true;
}

bool NzTerrainRenderer::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzTerrainRenderer::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	NazaraNotice("Uninitialized: TerrainRenderer module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzTerrainRenderer::s_moduleReferenceCounter = 0;
