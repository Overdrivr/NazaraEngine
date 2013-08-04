// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_DYNATERRAIN_HPP
#define NAZARA_ENUMS_DYNATERRAIN_HPP

enum nzLocation
{
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT
};

enum nzDirection
{
    TOP = 0,
    BOTTOM = 1,
    LEFT = 2,
    RIGHT = 3
};

enum nzConnectionType
{
  nzConnectionType_straight,
  nzConnectionType_orthogonal,
  nzConnectionType_reverse,
  nzConnectionType_none
};

enum nzQuadTreeType
{
    TERRAIN,
    PLANET
};

class NzTerrainChunksManager;

struct nzTerrainNodeData
{
    NzTerrainQuadTree* quadtree;
    NzTerrainChunksManager* chunksManager;
};
#endif // NAZARA_ENUMS_DYNATERRAIN_HPP
