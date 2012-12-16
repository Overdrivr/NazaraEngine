// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SPARSEBUFFER_HPP
#define SPARSEBUFFER_HPP

#include <Nazara/Math/Vector2.hpp>
#include <deque>
#include <map>
#include <list>

template <typename T> class NzSparseBuffer
{
    public:
        NzSparseBuffer(unsigned int bufferSize);
        ~NzSparseBuffer();

        unsigned int GetFreeSlotsAmount() const;

        //Insert the value inside the buffer
            //Returns -1 if something went wrong or the index location if everything ok
        int InsertValue(const T& value);

        //Reduces fragmentation by moving 1 value from case to an other
            //Returns a vector where x is the previous position
            //y the new position
            //If x == -1 then something went wrong and the buffer hasn't been changed
        NzVector2i ReduceFragmentation();

        //Returns true if everything ok
        bool RemoveValue(const T& value);

    protected:
    private:

        std::deque<nzBufferLocation> m_freeSlots;
        //Contient l'ensemble des id des patchs contenus par la zone et leur emplacement dans le buffer
            //Efficace pour trouver rapidement l'emplacement d'un patch
        std::map<id,nzBufferLocation> m_slots;
        //Représentation de l'agencement des patches dans les buffers
            //Efficace pour gérer la mémoire
        std::list<xid> m_internalBuffer;

        unsigned int m_bufferSize;
};

#include "SparseBuffer.inl"

#endif // SPARSEBUFFER_HPP
