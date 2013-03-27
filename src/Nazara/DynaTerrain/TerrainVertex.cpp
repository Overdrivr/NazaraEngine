// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainVertex.hpp>
#include <Nazara/DynaTerrain/DynaTerrainQuadTreeBase.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainVertex::NzTerrainVertex()
{
    m_isPositionInitialized = false;
    m_isNormalInitialized = false;
}

NzTerrainVertex::~NzTerrainVertex()
{

}

const NzVector3f& NzTerrainVertex::GetPosition() const
{
    return m_position;
}

const NzVector3f& NzTerrainVertex::GetNormal() const
{
    return m_normal;
}

void NzTerrainVertex::ComputeNormal(const NzTerrainVertex& v1, const NzTerrainVertex& v2, const NzTerrainVertex& v3,const NzTerrainVertex& v4)
{
    if(!m_isNormalInitialized)
    {
        NzVector3f v12,v23,v34,v41;

        v12 = v1.m_position.CrossProduct(v2.m_position);
        v23 = v2.m_position.CrossProduct(v3.m_position);
        v34 = v3.m_position.CrossProduct(v4.m_position);
        v41 = v4.m_position.CrossProduct(v1.m_position);

        m_normal = v12 + v23 + v34 + v41;
        m_normal.Normalize();
        m_isNormalInitialized = true;
    }
}

void NzTerrainVertex::ComputePosition(NzDynaTerrainQuadTreeBase* quadtree, const NzTerrainNodeID& ID, const NzVector2i& offset)
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
    m_isNormalInitialized = false;
}

bool NzTerrainVertex::IsInitialized() const
{
    return m_isPositionInitialized && m_isNormalInitialized;
}
