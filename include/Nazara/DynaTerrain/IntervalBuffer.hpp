// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Container
// Data is indexed by a unique key
// Container's size is fixed at construction
// Initially all slots are free
// Data can be added to an empty slot, and removed from a full one
// When two identical slots (i.e. both empty or full) or more are next to each other
// They form an "interval"
// An interval is described by a start index and a size
// It is possible to get a list of all full & empty intervals
// Example : used to keep track of occupied locations in a bix vertex buffer

#pragma once

#ifndef NAZARA_INTERVALBUFFER_HPP
#define NAZARA_INTERVALBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/DynaTerrain/Batch.hpp>
#include <vector>
#include <map>
#include <list>

template <typename T> class NzIntervalBuffer
{
    public:
        NzIntervalBuffer(unsigned int bufferSize);
        ~NzIntervalBuffer();

        //Returns the value's index OR -1 if the value cannot be located
        int FindValue(const T& value) const;

        unsigned int GetFilledSlotsAmount() const;
        const std::list<NzBatch>& GetFilledIntervals() const;
        std::list<NzBatch> GetFilledIntervalsCopy();// POURQUOI NOM DIFFERENT ? A TESTER
        const std::list<NzBatch>& GetFreeIntervals() const;
        std::list<NzBatch> GetFreeIntervalsCopy();//PAREIL
        unsigned int GetFreeSlotsAmount() const;

        //Insert the value's key inside the buffer
            //Returns -1 if something went wrong OR index location if everything ok
        int InsertValue(const T& value);

        //Reduces fragmentation by moving 1 value key from one case to an other
            //Returns a vector where x is the previous position
            //y the new position
            //If x == -1 then something went wrong and the buffer hasn't been changed
        NzVector2i ReduceFragmentation();
        //Returns the erased value key's index OR -1 if something went wrong
            //FIX ME : Mieux bool ou int en sortie ?
        int RemoveValue(const T& value);

    protected:
    private:
        bool AtomicKeyInsertion(std::list<NzBatch>& buffer, unsigned int index);
        bool AtomicKeyRemoval(std::list<NzBatch>& buffer, unsigned int index);

        //Contient l'ensemble des valeurs du buffer et leur emplacement dans le buffer
            //Efficace pour trouver rapidement l'emplacement d'une valeur dans internalBuffer
        std::map<T,int> m_slots;

        //Représentation des intervalles pleins dans le buffer
            //Efficace pour déterminer le nombre et la position de blocs de vertices consécutifs
            //x représente l'index
            //y le nombre de slots pleins consécutifs
        std::list<NzBatch> m_filledSlotBatches;

        //Représentation des espaces libres dans le buffer
            //x représente l'index
            //y le nombre de slots vides consécutifs
        std::list<NzBatch> m_freeSlotBatches;

        unsigned int m_bufferSize;
        unsigned int m_occupiedSlotsAmount;
};

#include "SparseBuffer.inl"

#endif // NAZARA_INDEXBUFFER_HPP
