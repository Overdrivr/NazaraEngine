// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SPARSEBUFFER_HPP
#define SPARSEBUFFER_HPP

#include <Nazara/Math/Vector2.hpp>
#include "Batch.hpp"
#include <vector>
#include <map>
#include <list>

template <typename T> class NzSparseBuffer
{
    public:
        NzSparseBuffer(unsigned int bufferSize);
        ~NzSparseBuffer();

        //Returns the value's index OR -1 if the value cannot be located
        int FindKey(const T& key) const;

        unsigned int GetFilledSlotsAmount() const;
        const std::list<NzVector2ui>& GetFilledBatches() const;
        std::list<NzVector2ui> GetFilledBatchesCopy();
        const std::list<NzVector2ui>& GetFreeBatches() const;
        std::list<NzVector2ui> GetFreeBatchesCopy();
        unsigned int GetFreeSlotsAmount() const;

        //Insert the value's key inside the buffer
            //Returns -1 if something went wrong OR index location if everything ok
        int InsertValueKey(const T& key);

        //Reduces fragmentation by moving 1 value key from one case to an other
            //Returns a vector where x is the previous position
            //y the new position
            //If x == -1 then something went wrong and the buffer hasn't been changed
        NzVector2i ReduceFragmentation();
        //Returns the erased value key's index OR -1 if something went wrong
            //FIX ME : Mieux bool ou int en sortie ?
        int RemoveValueKey(const T& key);

    protected:
    private:

        //Contient l'ensemble des valeurs du buffer et leur emplacement dans le buffer
            //Efficace pour trouver rapidement l'emplacement d'une valeur dans internalBuffer
        std::map<T,int> m_slots;

        //Représentation des espaces pleins dans le buffer
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

#endif // SPARSEBUFFER_HPP
