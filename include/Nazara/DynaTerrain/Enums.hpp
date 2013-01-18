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
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    VERTICAL,
    HORIZONTAL
};

struct id
{
    id() : lvl(0), sx(0), sy(0)
    {
    }
    id(unsigned int level, unsigned int sizex, unsigned int sizey) : lvl(level), sx(sizex), sy(sizey)
    {
    }
    unsigned int lvl;
    unsigned int sx;
    unsigned int sy;

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
