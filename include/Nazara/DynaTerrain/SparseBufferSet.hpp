// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NZSPARSEBUFFERSET_HPP
#define NZSPARSEBUFFERSET_HPP

#include "SparseBuffer.hpp"
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <vector>
#include <map>

template <typename T> class NzSparseBufferSet
{
    public:
        NzSparseBufferSet();
        ~NzSparseBufferSet();

        NzSparseBuffer<T>& at(unsigned int index);
        const NzSparseBuffer<T>& at(unsigned int index) const;
        void AddEmptyBuffer(unsigned int bufferSize);

        NzVector2i FindKeyLocation(const T& key) const;
        int FindKeyBuffer(const T& key) const;

        unsigned int GetBufferAmount() const;
        unsigned int GetFreeBuffersAmount() const;
        unsigned int GetTotalFreeSlotsAmount() const;
        unsigned int GetTotalOccupiedSlotsAmount() const;

        //x is the buffer
        //y is the index
        NzVector2i InsertValueKey(const T& key);
        //z is the destination buffer
        //w is the destination index
        NzVector4i ReduceFragmentation();
        bool RemoveBuffer(unsigned int index);
        bool RemoveValueKey(const T& key);

        NzVector2i UpdateValueKey(const T& key);

    protected:
    private:
        std::vector<NzSparseBuffer<T>> m_buffers;
        std::map<T,int> m_valueToBufferIndex;
        unsigned int m_occupiedSlotsAmount;
        unsigned int m_totalSlots;
};

#include "SparseBufferSet.inl"

#endif // NZSPARSEBUFFERSET_HPP
