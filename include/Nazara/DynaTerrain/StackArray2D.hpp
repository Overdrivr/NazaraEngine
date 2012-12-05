// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef STACKARRAY2D_HPP
#define STACKARRAY2D_HPP

#include <map>
#include <Nazara/Prerequesites.hpp>
#include "Enumerations.hpp"

template <typename T> class StackArray2D
{
    public:
        StackArray2D();
        ~StackArray2D() = default;

        const T& at(const id& ID) const;
        const T& at(unsigned int level, unsigned int i, unsigned int j) const;
        T& at(const id& ID);
        T& at(unsigned int level, unsigned int i, unsigned int j);

        int Erase(const id& ID);
        int Erase(unsigned int level, unsigned int i, unsigned int j);

        bool Exists(const id& ID) const;
        bool Exists(unsigned int level, unsigned int i, unsigned int j) const;

        void Display();

    protected:
    private:
        std::map<id, T> m_data;
        unsigned int m_level;
        unsigned int m_sizex;
        unsigned int m_sizey;
};

#include "StackArray2D.inl"

#endif // STACKARRAY2D_HPP
