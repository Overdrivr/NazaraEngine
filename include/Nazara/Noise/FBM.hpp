// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM_HPP
#define FBM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/CombinedNoiseBase.hpp>
#include <Nazara/Noise/Enums.hpp>

class NAZARA_API NzFBM : public NzCombinedNoiseBase
{
    public:
        NzFBM(NzNoiseBase& source);
        NzFBM(const NzFBM&) = delete;
        ~NzFBM() = default;

        float Get();

        NzFBM & operator=(const NzFBM&) = delete;

    protected:
    private:
        NzNoiseBase& m_source;
        float m_value;
        float m_remainder;
};

#endif // FBM_HPP
