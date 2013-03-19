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

enum nzQuadTreeType
{
    TERRAIN,
    PLANET
};

class NzDynaTerrainQuadTreeBase;
class NzTerrainMasterNode;

struct TerrainNodeData
{
    NzDynaTerrainQuadTreeBase* quadtree;
    NzTerrainMasterNode* dispatcher;
};
#endif // NAZARA_ENUMS_DYNATERRAIN_HPP
