// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINNODEID_HPP
#define NAZARA_TERRAINNODEID_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzTerrainNodeID
{
    public:

        NzTerrainNodeID();
        NzTerrainNodeID(int Depth, int locationx, int locationy);
        ~NzTerrainNodeID() = default;

        void FlipX();
        void FlipY();

        bool IsValid() const;
        void InvertXY();

        //TODO
        //Move(nzNeighbourDirection direction);

        void Normalize();//A RENOMMER ?

        NzTerrainNodeID& operator --();

        bool operator<( const NzTerrainNodeID& id ) const;

        int depth;
        int locx;
        int locy;
    private:
};

#endif // NAZARA_TERRAINNODEID_HPP
