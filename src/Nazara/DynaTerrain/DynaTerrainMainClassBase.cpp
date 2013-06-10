// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/DynaTerrainMainClassBase.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

NzDynaTerrainMainClassBase::NzDynaTerrainMainClassBase()
{
    m_faceFilling = nzFaceFilling_Fill;
}

NzDynaTerrainMainClassBase::~NzDynaTerrainMainClassBase()
{
    if(m_shader)
        delete m_shader;
    m_shader = nullptr;
    delete m_indexBuffer;
}

void NzDynaTerrainMainClassBase::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
    renderQueue.otherDrawables.push_back(this);
}

void NzDynaTerrainMainClassBase::BuildShader()
{
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
        NazaraError("Failed to load fragment shader");
        return;
    }

    if (!m_shader->Load(nzShaderType_Vertex, vertexSource))
    {
        NazaraError("Failed to load vertex shader");
        return;
    }

    if (!m_shader->Compile())
    {
        NazaraError("Failed to compile shader");
        return;
    }
}

void NzDynaTerrainMainClassBase::CreateIndexBuffer(unsigned int bufferCapacity, bool appendConfigurations)
{
    m_bufferCapacity = bufferCapacity;
    //On construit l'index buffer
    //taille totale : 1750 * 96 = 168000
    //taille atomique : 96
    //32 triangles

    //L'index pour deux triangles
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

    //L'index d'un patch entier
    unsigned int indexes[96];
    for(unsigned int i(0) ; i < 4 ; ++i)
        for(unsigned int j(0) ; j < 24 ; ++j)
        {
            indexes[i*24+j] = rowIndex[j] + i*5;
        }

    //L'index buffer en entier contenant "bufferCapacity" patchs
    std::vector<unsigned int> allIndexes;
    allIndexes.reserve(bufferCapacity * 96);
    for(int i(0) ; i < bufferCapacity ; ++i)
        for(int j(0) ; j < 96 ; ++j)
        {
            //allIndexes[i*96+j] = indexes[j] + 25*i;
            allIndexes.push_back(indexes[j] + 25 * i);
        }

	m_indexBuffer = new NzIndexBuffer(bufferCapacity * 96, true, nzBufferStorage_Hardware);
	if (!m_indexBuffer->Fill(allIndexes.data(), 0, bufferCapacity * 96)) // FIX ME : Que faire en cas d'échec
	{
		std::cout << "Failed to fill indexbuffer" << std::endl;
	}
}

const NzBoundingBoxf& NzDynaTerrainMainClassBase::GetBoundingBox() const
{
    return m_aabb;
}

nzSceneNodeType NzDynaTerrainMainClassBase::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}

void NzDynaTerrainMainClassBase::Draw() const
{
    if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

    nzUInt8 textureUnit;
	m_shader->SendTexture(m_shader->GetUniformLocation("terrainTexture"), &m_terrainTexture, &textureUnit);

    NzRenderer::SetMatrix(nzMatrixType_World,m_transformMatrix);
    NzRenderer::SetFaceCulling(nzFaceCulling_Back);
    NzRenderer::SetFaceFilling(m_faceFilling);
    NzRenderer::Enable(nzRendererParameter_FaceCulling, false);

    //NzRenderer::Enable(nzRendererParameter_Blend, false);
    NzRenderer::Enable(nzRendererParameter_DepthTest, true);
    //NzRenderer::Enable(nzRendererParameter_FaceCulling, false);
    NzRenderer::SetShader(m_shader);
    NzRenderer::SetTextureSampler(textureUnit, m_sampler);
    NzRenderer::SetIndexBuffer(m_indexBuffer);
}

void NzDynaTerrainMainClassBase::Initialize(const NzDynaTerrainConfigurationBase& configuration)
{
    BuildShader();

    if(!m_terrainTexture.LoadFromFile(configuration.groundTextures))
        std::cout<<"Could not load texture "<<configuration.groundTextures<<std::endl;

    //m_terrainTexture.EnableMipmapping(false);


    float radius = configuration.higherCameraPrecisionRadius;
    for(int i(0) ; i < configuration.cameraRadiusAmount ; ++i)
    {
        std::cout<<"radius "<<configuration.higherCameraPrecision - i<<" = "<<radius<<std::endl;
        radius *= configuration.radiusSizeIncrement;
    }

     m_sampler.SetWrapMode(nzSamplerWrap_Repeat);
}

void NzDynaTerrainMainClassBase::SetFaceFilling(const nzFaceFilling& faceFilling)
{
    m_faceFilling = faceFilling;
}

void NzDynaTerrainMainClassBase::Update(const NzVector3f& cameraPosition)
{

}

bool NzDynaTerrainMainClassBase::VisibilityTest(const NzFrustumf& frustum)
{
    //FIX ME
    return true;
}
