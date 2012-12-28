// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NZSPARSEBUFFERSET_HPP
#define NZSPARSEBUFFERSET_HPP

#include "SparseBuffer.hpp"
#include <Nazara/Math/Vector2.hpp>
#include <vector>

template <typename T> class NzSparseBufferSet
{
    public:
        NzSparseBufferSet();
        ~NzSparseBufferSet();

        T& at(unsigned int index);
        const T& at(unsigned int index) const;
        void AddEmptyBuffer();

        //nzBufferLocation FindValue(const T& value) const;

        unsigned int GetBufferAmount() const;
        unsigned int GetFreeBuffersAmount() const;
        unsigned int GetTotalFreeSlotsAmount() const;
        unsigned int GetTotalOccupiedSlotsAmount() const;

        nzBufferLocation InsertValue(const T& value);

        NzVector2<nzBufferLocation> ReduceFragmentation();
        bool RemoveBuffer(unsigned int index);
        bool RemoveValue(const T& value);

        nzBufferLocation UpdateValue(const T& value);

    protected:
    private:
        std::vector<NzSparseBuffer<T>> m_buffers;
        unsigned int m_occupiedSlotsAmount;
};

#include "SparseBufferSet.inl"

#endif // NZSPARSEBUFFERSET_HPP
