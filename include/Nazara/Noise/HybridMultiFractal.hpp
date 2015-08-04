// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HYBRIDMULTIFRACTAL_HPP
#define HYBRIDMULTIFRACTAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/CombinedNoiseBase.hpp>
#include <Nazara/Noise/Enums.hpp>

class NAZARA_API NzHybridMultiFractal : public NzCombinedNoiseBase
{
    public:
        NzHybridMultiFractal(NzNoiseBase& source);
        NzHybridMultiFractal(const NzHybridMultiFractal&) = delete;
        ~NzHybridMultiFractal() = default;

        float Get();

        NzHybridMultiFractal & operator=(const NzHybridMultiFractal&) = delete;

    protected:
    private:
        NzNoiseBase& m_source;
        float m_value;
        float m_remainder;
        float m_offset;
        float m_weight;
        float m_signal;
};

#endif // HYBRIDMULTIFRACTAL_HPP

