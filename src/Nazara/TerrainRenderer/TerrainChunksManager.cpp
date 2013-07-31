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

    for(unsigned int i(0) ; i < m_depth*m_depth ; ++i)
        m_chunks.emplace_back();
}

/*
void NzTerrainMasterNode::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
    renderQueue.otherDrawables.push_back(static_cast<NzDrawable>(this));
}
*/

NzTerrainChunk* NzTerrainChunksManager::LocateChunk(NzVector2f location)
{
    return &(m_chunks.at(location.x + location.y * m_depth));
}

void NzTerrainChunksManager::DrawChunks() const
{
    for(unsigned int i(0) ; i < m_depth*m_depth ; ++i)
    {
        NzTerrainRenderer::DrawTerrainChunk(m_chunks.at(i));
    }
}

const NzBoundingVolumef& NzTerrainChunksManager::GetGlobalBoundingBox() const
{
    return m_aabb;
}

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
bool NzTerrainMasterNode::RemovePatch(const NzTerrainNodeID& ID)
{
    if(m_isReady)
    {
        //On récupère la zone devant accueillir le patch
        NzTerrainNodeID temp;
        temp.locx = ID.locx * m_zonesAmountX/std::pow(2,ID.depth);
        temp.locy = ID.locy * m_zonesAmountX/std::pow(2,ID.depth);

        if(temp.locx < m_zonesAmountX && temp.locy < m_zonesAmountX)
        {
            //std::cout<<"submitting patch to zone "<<temp.sx<<" | "<<temp.sy<<std::endl;
            //std::cout<<"Trying Removing patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<" in Zone "<<temp.lvl<<"|"<<temp.sx<<"|"<<temp.sy<<" with buf"<<std::endl;
            m_zones.at(temp.locx + m_zonesAmountX * temp.locy)->RemovePatch(ID);
            return true;
        }
        else
        {
            std::cout<<"Removing patch "<<ID.depth<<"|"<<ID.locx<<"|"<<ID.locy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
            return false;
        }
    }
    std::cout<<"Impossible to remove patch"<<std::endl;
    return false;
}

bool NzTerrainMasterNode::Initialize(unsigned int zoneDepth)
{
    ///---- On crée la déclaration de vertices


    ///-----On crée toutes les zones nécessaires
    m_zoneDepth = zoneDepth;
    m_zonesAmountX = std::pow(2,m_zoneDepth);
    //On crée le nombre de zones demandé
    for(unsigned int i(0) ; i < m_zonesAmountX * m_zonesAmountX ; ++i)
    {
        std::unique_ptr<NzTerrainNode> zone(new NzTerrainNode(this,m_patchAmount));
        m_zones.push_back(std::move(zone));
    }

    m_isReady = true;
    return true;
}

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

bool NzTerrainMasterNode::SubmitPatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID)
{
    if(!m_isReady)
        return false;

    //On récupère la zone devant accueillir le patch
    NzTerrainNodeID temp;
    temp.locx = ID.locx * m_zonesAmountX / std::pow(2,ID.depth);
    temp.locy = ID.locy * m_zonesAmountX / std::pow(2,ID.depth);

    if(temp.locx < m_zonesAmountX && temp.locy < m_zonesAmountX)
    {
        m_zones.at(temp.locx + m_zonesAmountX * temp.locy)->AddPatch(subBuffer,ID);
        return true;
    }
    else
    {
        std::cout<<"Submitting patch "<<ID.depth<<"|"<<ID.locx<<"|"<<ID.locy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
        return false;
    }
}

bool NzTerrainMasterNode::UpdatePatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID)
{
    if(m_isReady)
    {
        //On récupère la zone devant accueillir le patch
        NzTerrainNodeID temp;
        temp.locx = ID.locx * m_zonesAmountX / std::pow(2,ID.depth);
        temp.locy = ID.locy * m_zonesAmountX / std::pow(2,ID.depth);

        if(temp.locx < m_zonesAmountX && temp.locy < m_zonesAmountX)
        {
            m_zones.at(temp.locx + m_zonesAmountX * temp.locy)->UpdatePatch(subBuffer,ID);
            return true;
        }
        else
        {
            std::cout<<"Updating patch "<<ID.depth<<"|"<<ID.locx<<"|"<<ID.locy<<" outside supported area :"<<m_zonesAmountX<<std::endl;
            return false;
        }
    }
    else
        return false;
}
*/

/*
virtual bool NzTerrainMasterNode::VisibilityTest(const NzFrustumf& frustum)
{
    //FIX ME
    return true;
}
*/
