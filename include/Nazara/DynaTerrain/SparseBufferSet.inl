// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <iostream>

template <typename T>
NzSparseBufferSet<T>::NzSparseBufferSet()
{
    m_occupiedSlotsAmount = 0;
    m_totalSlots = 0;
}

template <typename T>
NzSparseBufferSet<T>::~NzSparseBufferSet()
{
    //dtor
}

template <typename T>
NzSparseBuffer<T>& NzSparseBufferSet<T>::at(unsigned int index)
{
    return m_buffers.at(index);
}

template <typename T>
const NzSparseBuffer<T>& NzSparseBufferSet<T>::at(unsigned int index) const
{
    return m_buffers.at(index);
}

template <typename T>
void NzSparseBufferSet<T>::AddEmptyBuffer(unsigned int bufferSize)
{
    m_totalSlots += bufferSize;
    NzSparseBuffer<T> temp(bufferSize);
    m_buffers.push_back(temp);
}

template <typename T>
NzVector2i NzSparseBufferSet<T>::FindKeyLocation(const T& key) const
{
    NzVector2i location(-1,-1);

    location.x = FindKeyBuffer(key);

    if(location.x > -1)
    {
       location.y = m_buffers.at(location.x).FindKey(key);
    }

    return location;
}

template <typename T>
int NzSparseBufferSet<T>::FindKeyBuffer(const T& key) const
{
    typename std::map<T,int>::const_iterator it = m_valueToBufferIndex.find(key);

    if(it == m_valueToBufferIndex.end())
        return -1;

    int bufferIndex = (*it).second;

    if(bufferIndex < 0 || bufferIndex >= static_cast<int>(m_buffers.size()))
    {
        std::cout<<"SparseBufferSet::FindKeyBuffer : bufferIndex outside boundaries : "<<bufferIndex<<" | 0 to "<<m_buffers.size()<<"|"<<std::endl;
        return -1;
    }

    return bufferIndex;
}

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
    return m_totalSlots - m_occupiedSlotsAmount;
}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetTotalOccupiedSlotsAmount() const
{
    return m_occupiedSlotsAmount;
}

template <typename T>
NzVector2i NzSparseBufferSet<T>::InsertValueKey(const T& key)
{
    NzVector2i location(-1,-1);

    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {
        if(m_buffers.at(i).GetFreeSlotsAmount() > 0)
        {
            location.y =  m_buffers.at(i).InsertValueKey(key);
            if(location.y > -1)//Si l'index retourné est valide, on s'arrête là
            {
                m_valueToBufferIndex[key] = i;
                location.x = i;
                break;
            }
            //Sinon on continue
        }
    }
    m_occupiedSlotsAmount++;
    return location;
}

template <typename T>
NzVector4i NzSparseBufferSet<T>::ReduceFragmentation()
{
    NzVector4i location(-1,-1,-1,-1);

    return location;
}

template <typename T>
bool NzSparseBufferSet<T>::RemoveBuffer(unsigned int index)
{
    //m_totalSlots -= m_buffers.at(index).size();
    return false;
}

template <typename T>
NzVector2i NzSparseBufferSet<T>::RemoveValueKey(const T& key)
{
    NzVector2i location(-1,-1);
    location.x = FindKeyBuffer(key);

    if(location.x < 0)
        return NzVector2i(-1,-1);

    location.y = m_buffers.at(location.x).RemoveValueKey(key);

    if(location.y > -1)
    {
        m_occupiedSlotsAmount--;
        m_valueToBufferIndex.erase(key);
        return location;
    }

    return NzVector2i(-1,-1);
}

template <typename T>
NzVector2i NzSparseBufferSet<T>::UpdateValueKey(const T& key)
{
    NzVector2i location(-1,-1);

    return location;
}
