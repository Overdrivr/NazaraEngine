// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainVertex.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainVertex::NzTerrainVertex()
{
    m_isPositionInitialized = false;
}

NzTerrainVertex::~NzTerrainVertex()
{

}

const NzVector3f& NzTerrainVertex::GetPosition() const
{
    return m_position;
}

void NzTerrainVertex::ComputePosition(NzTerrainQuadTree* quadtree, const NzTerrainNodeID& ID, const NzVector2i& offset)
{
    if(!m_isPositionInitialized)
    {
        m_position = quadtree->GetVertexPosition(ID,offset.x,offset.y);
        m_isPositionInitialized = true;
    }
}

void NzTerrainVertex::Invalidate()
{
    m_isPositionInitialized = false;
}

bool NzTerrainVertex::IsInitialized() const
{
    return m_isPositionInitialized;
}
