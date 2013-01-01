// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SPARSEBUFFER_HPP
#define SPARSEBUFFER_HPP

#include <Nazara/Math/Vector2.hpp>
#include <vector>
#include <map>
#include <list>

template <typename T> class NzSparseBuffer
{
    public:
        NzSparseBuffer(unsigned int bufferSize);
        ~NzSparseBuffer();

        //Returns the value's index OR -1 if the value cannot be located
        int FindValue(const T& value);

        const std::list<NzVector2ui>& GetVerticeIndexBatches();
        unsigned int GetFreeSlotsAmount() const;

        //Insert the value inside the buffer
            //Returns -1 if something went wrong OR index location if everything ok
        int InsertValue(const T& value);

        //Reduces fragmentation by moving 1 value from case to an other
            //Returns a vector where x is the previous position
            //y the new position
            //If x == -1 then something went wrong and the buffer hasn't been changed
        NzVector2i ReduceFragmentation();
        bool RemoveValue(const T& value);

    protected:
    private:

        //Contient l'ensemble des valeurs du buffer et leur emplacement dans le buffer
            //Efficace pour trouver rapidement l'emplacement d'une valeur dans internalBuffer
        std::map<T,int> m_slots;

        //Map du buffer...utile ? Sachant que ce buffer n'est qu'une repr�sentation et non un "vrai" buffer
        std::vector<T> m_internalBuffer;

        //Repr�sentation des espaces pleins dans le buffer
            //Efficace pour d�terminer le nombre et la position de blocs de vertices cons�cutifs
            //x repr�sente l'index
            //y le nombre de slots pleins cons�cutifs
        std::list<NzVector2ui> m_filledSlotBatches;

        //Repr�sentation des espaces libres dans le buffer
            //x repr�sente l'index
            //y le nombre de slots vides cons�cutifs
        std::list<NzVector2ui> m_freeSlotBatches;

        unsigned int m_bufferSize;
        unsigned int m_occupiedSlotsAmount;
};

#include "SparseBuffer.inl"

#endif // SPARSEBUFFER_HPP
