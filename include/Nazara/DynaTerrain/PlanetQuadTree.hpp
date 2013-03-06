// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLANETQUADTREE_HPP
#define NAZARA_PLANETUADTREE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/TerrainNodeID.hpp>

class NAZARA_API NzPlanetQuadTree : public NzTerrainQuadTree
{
    public:
        NzPlanetQuadTree(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource);
        ~NzPlanetQuadTree();

        virtual NzVector3f GetVertexPosition(const NzTerrainNodeID& nodeID, int x, int y);

    protected:
    private:
};

#endif // NAZARA_PLANETQUADTREE_HPP
