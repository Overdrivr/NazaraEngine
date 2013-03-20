// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/DynaTerrain/TerrainMasterNode.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <iostream>
#include <cmath>

NzTerrainMasterNode::NzTerrainMasterNode(unsigned int patchBufferSize)
{
    m_patchSize = 600;
    m_bufferSize = patchBufferSize * m_patchSize;
    m_patchAmount = patchBufferSize;
    m_isReady = false;
}

NzTerrainMasterNode::~NzTerrainMasterNode()
{
    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {
        delete m_buffers.at(i);
        m_buffers.at(i) = nullptr;
    }
    m_buffers.clear();
}
/*
void NzTerrainMasterNode::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
    renderQueue.otherDrawables.push_back(static_cast<NzDrawable>(this));
}
*/
void NzTerrainMasterNode::Draw() const
{
    if(m_isReady)
    {
        for(unsigned int i(0) ; i < m_zones.size() ; ++i)
        {
            m_zones.at(i)->DrawBuffers();
        }
    }
}
/*
const NzBoundingBoxf& NzTerrainMasterNode::GetBoundingBox() const
{
    return m_aabb;
}
*/
unsigned int NzTerrainMasterNode::GetFreeBuffersAmount() const
{
    return m_freeBuffers.size();
}
/*
nzSceneNodeType NzTerrainMasterNode::GetSceneNodeType() const
{
    return nzSceneNodeType_User;
}*/

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
/*
virtual bool NzTerrainMasterNode::VisibilityTest(const NzFrustumf& frustum)
{
    //FIX ME
    return true;
}
*/
