// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine - Terrain renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/TerrainRenderer/TerrainRenderer.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/TerrainRenderer/Config.hpp>
#include <Nazara/TerrainRenderer/Debug.hpp>

namespace
{
    //La déclaration de structure de tous les vertex buffer
    NzVertexDeclaration m_declaration;
    NzIndexBuffer* m_indexBuffer;
    NzShader* m_shader;
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

const NzIndexBuffer& NzTerrainRenderer::GetIndexBuffer()
{
    return *m_indexBuffer;
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

    /// La structure du vertex buffer
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

	///L'index buffer
    unsigned int rowIndex[24];

    for(int i(0) ; i < 4 ; ++i)
    {
        rowIndex[i*6] = i;
        rowIndex[i*6+1] = i + 6;
        rowIndex[i*6+2] = i + 1;
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

    std::vector<unsigned int> allIndexes;
    allIndexes.reserve(VERTEX_BUFFER_SLOT_AMOUNT * 96);

    for(int i(0) ; i < VERTEX_BUFFER_SLOT_AMOUNT ; ++i)
        for(int j(0) ; j < 96 ; ++j)
        {
            allIndexes.push_back(indexes[j] + 25 * i);
        }

	m_indexBuffer = new NzIndexBuffer(VERTEX_BUFFER_SLOT_AMOUNT * 96, true, nzBufferStorage_Hardware);

	if (!m_indexBuffer->Fill(allIndexes.data(), 0, VERTEX_BUFFER_SLOT_AMOUNT * 96))
	{
		NazaraError("Failed to initialize terrain renderer module : Failed to create index buffer");
		delete m_indexBuffer;
		return false;
	}

	/// Le shader par défaut du terrain
	const char* vertexSource =
    "#version 140\n"
    "in vec3 VertexPosition;\n"
    "in vec3 VertexNormal;\n"
    "uniform mat4 WorldViewProjMatrix;\n"
    "out vec3 normal;\n"
    "out vec3 position;\n"

    "void main()\n"
    "{\n"
	"normal = VertexNormal;\n"
	"position = VertexPosition;\n"
    "gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);\n"
    "}\n";

    const char* fragmentSource =
    "#version 140\n"
    "uniform sampler2D terrainTexture;\n"
    "out vec4 out_Color;\n"
    "in vec3 normal;\n"
    "in vec3 position;\n"
    "vec2 uvTileConversion(float slope, float altitude, vec2 uv);\n"
    "void main()\n"
    "{\n"
    "vec3 upVector = vec3(0.0,1.0,0.0);\n"
    "float slope = dot(normal,upVector);\n"
    "float altitude = position.y;\n"
    "float tex_scale = 512.0;\n"
    "vec3 uvw = position/tex_scale;\n"

    "vec3 weights = abs(normal);\n"
    "weights = max((weights - 0.2) * 5 ,0);\n"
    "weights /= vec3(weights.x + weights.y + weights.z);\n"

    "vec2 coord1 = uvw.zy;\n"
    "vec2 coord2 = uvw.xz;\n"
    "vec2 coord3 = uvw.yx;\n"

    "vec4 col1 = texture2D(terrainTexture,coord1);\n"
    "vec4 col2 = texture2D(terrainTexture,coord2);\n"
    "vec4 col3 = texture2D(terrainTexture,coord3);\n"

    "out_Color = col1 * weights.xxxx + col2 * weights.yyyy + col3 * weights.zzzz;\n"
    "}\n"
    "vec2 uvTileConversion(float slope, float altitude, vec2 uv)\n"
    "{\n"
    "vec2 tile = vec2(0.0,3.0);\n"
    "if(altitude > 600.0)\n"
    "    tile = vec2(3.0,3.0);\n"
    "else if(altitude < 75.0)\n"
    "    tile = vec2(3.0,2.0);\n"
    "else if(slope > 0.5)\n"
    "    tile = vec2(2.0,3.0);\n"

    "vec2 newUV;\n"
    "newUV.x = uv.x*0.25 + tile.x*0.25;\n"
    "newUV.y = uv.y*0.25 + tile.y*0.25;\n"
    "return newUV;\n"
    "}\n";

    m_shader = new NzShader(nzShaderLanguage_GLSL);

    if (!m_shader->Load(nzShaderType_Fragment, fragmentSource))
    {
        NazaraError("Failed to initialize terrain renderer module : Failed to load fragment shader");
        delete m_indexBuffer;
        delete m_shader;
        return false;
    }

    if (!m_shader->Load(nzShaderType_Vertex, vertexSource))
    {
        NazaraError("Failed to initialize terrain renderer module : Failed to load vertex shader");
        delete m_indexBuffer;
        delete m_shader;
        return false;
    }

    if (!m_shader->Compile())
    {
        NazaraError("Failed to initialize terrain renderer module : Failed to compile shader");
        delete m_indexBuffer;
        delete m_shader;
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

    delete m_indexBuffer;
    delete m_shader;

	NazaraNotice("Uninitialized: TerrainRenderer module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzTerrainRenderer::s_moduleReferenceCounter = 0;
