// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainNodeDirection.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

NzTerrainNodeDirection::NzTerrainNodeDirection()
{
    m_direction = TOP;
}

NzTerrainNodeDirection::NzTerrainNodeDirection(nzDirection direction)
{
    m_direction = direction;
}

nzDirection NzTerrainNodeDirection::GetReversed() const
{
    nzDirection temp;
    switch(m_direction)
    {
        case TOP :
            temp = BOTTOM;
        break;

        case RIGHT :
            temp = LEFT;
        break;

        case BOTTOM :
            temp = TOP;
        break;

        case LEFT :
            temp = RIGHT;
        break;
    }
    return temp;
}

void NzTerrainNodeDirection::Reverse()
{
    m_direction = GetReversed();
}

nzDirection& NzTerrainNodeDirection::operator()()
{
    return m_direction;
}
