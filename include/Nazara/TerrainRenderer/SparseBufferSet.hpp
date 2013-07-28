// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Terrain Renderer module".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPARSEBUFFERSET_HPP
#define NAZARA_SPARSEBUFFERSET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/TerrainRenderer/IntervalBuffer.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <vector>
#include <map>

//TODO : AJouter Get free slot, FillFreeSlot, FreeFilledSlot() DoesKeyExist()
//TODO : Un free slot renvoyé doit toujours être valide, le type de retour doit tjrs être unsigned int
template <typename T> class NzSparseBufferSet
{
    public:
        NzSparseBufferSet();
        ~NzSparseBufferSet() = default;

        NzIntervalBuffer<T>& at(unsigned int index);
        const NzIntervalBuffer<T>& at(unsigned int index) const;
        void AddEmptyBuffer(unsigned int bufferSize);

        bool DoesKeyExists(const T& key);

        bool FillFreeSlot(const NzVector2i& slot, const T& value);
        bool FreeFilledSlot(const NzVector2i& slot);//TODO : a terme ne devrait plus avoir besoin de value
        NzVector2i FindKey(const T& key) const;
        int FindKeyBuffer(const T& key) const;

        NzVector2i GetFreeSlot() const;//TODO
        unsigned int GetBuffersAmount() const;
        unsigned int GetFreeBuffersAmount() const;
        unsigned int GetTotalFreeSlotsAmount() const;
        unsigned int GetTotalOccupiedSlotsAmount() const;

        //x is the buffer
        //y is the index
        //NzVector2i InsertValueKey(const T& key);
        //z is the destination buffer
        //w is the destination index
        NzVector4i ReduceFragmentation();
        bool RemoveBuffer(unsigned int index);
        //NzVector2i RemoveValueKey(const T& key);

        //NzVector2i UpdateValueKey(const T& key);

    protected:
    private:
        std::vector<NzIntervalBuffer<T>> m_buffers;
        std::map<T,int> m_valueToBufferIndex;
        unsigned int m_occupiedSlotsAmount;
        unsigned int m_totalSlots;
};

#include "SparseBufferSet.inl"

#endif // NAZARA_SPARSEBUFFERSET_HPP
