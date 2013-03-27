// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINVERTEX_HPP
#define NAZARA_TERRAINVERTEX_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>

class NzTerrainQuadTree;
class NzTerrainNodeID;

class NAZARA_API NzTerrainVertex
{
    public:
        NzTerrainVertex();
        ~NzTerrainVertex();

        const NzVector3f& GetPosition() const;
        const NzVector3f& GetNormal() const;

        void ComputePosition(NzTerrainQuadTree* quadtree, const NzTerrainNodeID& ID, const NzVector2i& offset);
        void ComputeNormal(const NzTerrainVertex& v1,
                           const NzTerrainVertex& v2,
                           const NzTerrainVertex& v3,
                           const NzTerrainVertex& v4);
        void Invalidate();
        bool IsInitialized() const;

    protected:
    private:

        NzVector3f m_position;
        NzVector3f m_normal;
        bool m_isPositionInitialized;
        bool m_isNormalInitialized;
};

#endif // NAZARA_TERRAINVERTEX_HPP
