// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

template <typename T>
NzSparseBufferSet<T>::NzSparseBufferSet()
{
    m_occupiedSlotsAmount = 0;
}

template <typename T>
NzSparseBufferSet<T>::~NzSparseBufferSet()
{
    //dtor
}

template <typename T>
T& NzSparseBufferSet<T>::at(unsigned int index)
{
    return m_buffers.at(index);
}

template <typename T>
const T& NzSparseBufferSet<T>::at(unsigned int index) const
{
    return m_buffers.at(index);
}

template <typename T>
void NzSparseBufferSet<T>::AddEmptyBuffer()
{
    NzSparseBuffer<T> temp(1750);
    m_buffers.push_back(temp);
}
/*
template <typename T>
nzBufferLocation NzSparseBufferSet<T>::FindValue(const T& value) const
{
    nzBufferLocation
    for(unsigned int i(0) ; i < m_buffers.size(); ++i)
    {
       m_buffers.at(i).FindValue(value);
    }
}
*/
template <typename T>
unsigned int NzSparseBufferSet<T>::GetBufferAmount() const
{
    return m_buffers.size();
}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetFreeBuffersAmount() const
{
    //FIX ME
    return 0;
}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetTotalFreeSlotsAmount() const
{
    return m_buffers.size()*1750-m_occupiedSlotsAmount;
}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetTotalOccupiedSlotsAmount() const
{
    return m_occupiedSlotsAmount;
}

template <typename T>
nzBufferLocation NzSparseBufferSet<T>::InsertValue(const T& value)
{
    nzBufferLocation location;
    location.index = -1;
    location.buffer = -1;

    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {
        if(m_buffers.at(i).GetFreeSlotsAmount > 0)
        {
            location.buffer = i;
            location.index =  m_buffers.at(i).InsertValue(value);
            return location;
        }
    }
}

template <typename T>
NzVector2<nzBufferLocation> NzSparseBufferSet<T>::ReduceFragmentation()
{
    nzBufferLocation location;
    location.index = -1;
    location.buffer = -1;

    return location;
}

template <typename T>
bool NzSparseBufferSet<T>::RemoveBuffer(unsigned int index)
{
    return false;
}

template <typename T>
bool NzSparseBufferSet<T>::RemoveValue(const T& value)
{
    return false;
}

template <typename T>
nzBufferLocation NzSparseBufferSet<T>::UpdateValue(const T& value)
{
    nzBufferLocation location;
    location.index = -1;
    location.buffer = -1;

    return location;
}
