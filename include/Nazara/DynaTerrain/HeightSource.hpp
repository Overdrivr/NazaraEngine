// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HEIGHTSOURCE_HPP
#define NAZARA_HEIGHTSOURCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <vector>

class NAZARA_API NzHeightSource
{
    public:
        NzHeightSource();
        ~NzHeightSource();
        float GetHeight(float x, float y);
        bool LoadTerrainFile(const NzString& filename);
        bool LoadChunkFile();
    protected:
        virtual float GetNoiseValue(float x, float y) = 0;
    private:
        //File structure version
        nzUInt8 m_structureVersion;
        //Properties
        bool m_bicubicInterpolation;
        bool m_proceduralDetails;
        float m_chunkSize;
        nzUInt32 m_chunksNumber;
        std::vector<NzString> m_chunkPatches;
        NzString m_filepath;

};

#endif // NAZARA_HEIGHTSOURCE_HPP
