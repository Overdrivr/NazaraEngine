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
    RIGHT = 3,
    VERTICAL = 4,
    HORIZONTAL = 5
};

struct id
{
    id() : lvl(0), sx(0), sy(0)
    {
    }
    id(int level, int sizex, int sizey) : lvl(level), sx(sizex), sy(sizey)
    {
    }
    int lvl;
    int sx;
    int sy;

    bool operator<( const id& ID ) const
    {
        return (this->lvl != ID.lvl) ? this->lvl < ID.lvl : (this->sx != ID.sx) ? this->sx < ID.sx : this->sy < ID.sy;
    }
};

class NzTerrainQuadTree;
class NzHeightSource;
class NzDispatcher;

struct TerrainNodeData
{
    NzTerrainQuadTree* quadtree;
    NzHeightSource* heightSource;
    NzDispatcher* dispatcher;
};

#endif // NAZARA_ENUMS_DYNATERRAIN_HPP
