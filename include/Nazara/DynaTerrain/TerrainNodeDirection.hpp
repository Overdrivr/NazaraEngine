// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINNODEDIRECTION_HPP
#define NAZARA_TERRAINNODEDIRECTION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>

class NAZARA_API NzTerrainNodeDirection
{
    public:

        NzTerrainNodeDirection();
        NzTerrainNodeDirection(nzDirection direction);
        ~NzTerrainNodeDirection() = default;

        nzDirection GetReversed() const;

        void Reverse();

        nzDirection& operator()();

    private:
        nzDirection m_direction;

};

#endif // NAZARA_TERRAINNODEDIRECTION_HPP
