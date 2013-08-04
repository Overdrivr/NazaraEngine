// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/TerrainRenderer/TerrainChunksManager.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <cmath>

NzTerrainChunksManager::NzTerrainChunksManager(float edgelenght, unsigned int depth)
{
    m_edgeLenght = edgelenght;
    m_depth = depth;
    m_gridStep = m_edgeLenght / m_depth;//std::pow(m_depth,2); ??

    for(unsigned int i(0) ; i < m_depth * m_depth ; ++i)
        m_chunks.emplace_back();
}

bool NzTerrainChunksManager::AddMesh(const std::array<float,150>& vertexData, const NzBoundingVolumef& meshBoundingBox, NzTerrainNodeID meshIdentifiant)
{
    // On trouve le chunk devant accueillir le mesh
    if(meshBoundingBox.aabb.x < 0 || meshBoundingBox.aabb.x > m_edgeLenght ||
       meshBoundingBox.aabb.y < 0 || meshBoundingBox.aabb.y > m_edgeLenght)
    {
        NazaraError("NzTerrainChunksManager::AddMesh : Mesh location outside supported area");
        return false;
    }

    unsigned int x = static_cast<int>(meshBoundingBox.aabb.x / m_gridStep);
    unsigned int y = static_cast<int>(meshBoundingBox.aabb.y / m_gridStep);

    m_chunks.at(x + m_depth * y).AddMesh(vertexData,meshBoundingBox,meshIdentifiant);
    return true;
}

void NzTerrainChunksManager::DrawChunks() const
{
    for(unsigned int i(0) ; i < m_depth * m_depth ; ++i)
    {
        NzTerrainRenderer::DrawTerrainChunk(m_chunks.at(i));
    }
}

const NzBoundingVolumef& NzTerrainChunksManager::GetGlobalBoundingBox() const
{
    return m_aabb;
}

NzTerrainChunk* NzTerrainChunksManager::LocateChunk(NzVector2f location)
{
    return &(m_chunks.at(location.x + m_depth * location.y));
}

bool NzTerrainChunksManager::UpdateMesh(const std::array<float,150>& vertexData, const NzBoundingVolumef& meshBoundingBox, NzTerrainNodeID meshIdentifiant)
{
    // On trouve le chunk devant accueillir le mesh
    if(meshBoundingBox.aabb.x < 0 || meshBoundingBox.aabb.x > m_edgeLenght ||
       meshBoundingBox.aabb.y < 0 || meshBoundingBox.aabb.y > m_edgeLenght)
    {
        NazaraError("NzTerrainChunksManager::AddMesh : Mesh location outside supported area");
        return false;
    }

    unsigned int x = static_cast<int>(meshBoundingBox.aabb.x / m_gridStep);
    unsigned int y = static_cast<int>(meshBoundingBox.aabb.y / m_gridStep);

    m_chunks.at(x + m_depth * y).UpdateMesh(vertexData,meshIdentifiant);
    return true;
}

bool NzTerrainChunksManager::RemoveMesh(const NzBoundingVolumef& meshBoundingBox, NzTerrainNodeID meshIdentifiant)
{
    // On trouve le chunk devant accueillir le mesh
    if(meshBoundingBox.aabb.x < 0 || meshBoundingBox.aabb.x > m_edgeLenght ||
       meshBoundingBox.aabb.y < 0 || meshBoundingBox.aabb.y > m_edgeLenght)
    {
        NazaraError("NzTerrainChunksManager::AddMesh : Mesh location outside supported area");
        return false;
    }

    unsigned int x = static_cast<int>(meshBoundingBox.aabb.x / m_gridStep);
    unsigned int y = static_cast<int>(meshBoundingBox.aabb.y / m_gridStep);

    m_chunks.at(x + m_depth * y).RemoveMesh(meshIdentifiant);
    return true;
}

        /*
void NzTerrainMasterNode::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
    renderQueue.otherDrawables.push_back(static_cast<NzDrawable>(this));
}
*/

/*
unsigned int NzTerrainMasterNode::GetFreeBuffersAmount() const
{
    return m_freeBuffers.size();
}

nzSceneNodeType NzTerrainMasterNode::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}*/
/*
NzVertexBuffer* NzTerrainMasterNode::QueryFreeBuffer()
{
    NzVertexBuffer* buffer = nullptr;

    if(m_freeBuffers.size() > 0)
    {
        buffer = m_freeBuffers.front();
        m_freeBuffers.pop();
    }
    else
    {
        //std::cout<<"NzDispatcher::QueryFreeBuffer : Allocated New VertexBuffer"<<std::endl;
        buffer = new NzVertexBuffer(&m_declaration,m_bufferSize, nzBufferStorage_Hardware, nzBufferUsage_Static);
        m_buffers.push_back(buffer);
    }

    return buffer;
}

void NzTerrainMasterNode::ReturnBuffer(NzVertexBuffer* buffer)
{
    m_freeBuffers.push(buffer);
}

*/
