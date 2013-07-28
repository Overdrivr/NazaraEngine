// Copyright (C) 2012 RÃ©mi BÃ¨ges
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
NzIntervalBuffer<T>& NzSparseBufferSet<T>::at(unsigned int index)
{
    return m_buffers.at(index);
}

template <typename T>
const NzIntervalBuffer<T>& NzSparseBufferSet<T>::at(unsigned int index) const
{
    return m_buffers.at(index);
}

template <typename T>
void NzSparseBufferSet<T>::AddEmptyBuffer(unsigned int bufferSize)
{
    m_totalSlots += bufferSize;
    NzIntervalBuffer<T> temp(bufferSize);
    m_buffers.push_back(temp);
}

template <typename T>
bool NzSparseBufferSet<T>::DoesKeyExists(const T& key)
{
    NzVector2i location(-1,-1);

    location.x = FindKeyBuffer(key);

    if(location.x < 0)
        return false;

    location.y = m_buffers.at(location.x).FindValue(key);

    if(location.y < 0)
        return false;

    return true;
}

template <typename T>
NzVector2i NzSparseBufferSet<T>::FindKey(const T& key) const
{
    NzVector2i location(-1,-1);

    location.x = FindKeyBuffer(key);

    if(location.x > -1)
    {
       location.y = m_buffers.at(location.x).FindValue(key);
    }

    return location;
}

template <typename T>
bool NzSparseBufferSet<T>::FillFreeSlot(const NzVector2i& slot, const T& value)
{
    NzVector2i location(-1,-1);

    for(unsigned int i(0) ; i < m_buffers.size() ; ++i)
    {
        if(m_buffers.at(i).GetFreeSlotsAmount() > 0)
        {
            location.y =  m_buffers.at(i).InsertValue(value);

            if(location.y > -1)
            {
                m_valueToBufferIndex[value] = i;
                location.x = i;
                m_occupiedSlotsAmount++;
                return true;
            }
        }
    }

    return false;
}

template <typename T>
bool NzSparseBufferSet<T>::FreeFilledSlot(const NzVector2i& slot)
{
    if(slot.x < 0)
        return false;
    if(slot.x >= m_buffers.size())
        return false;
    if(slot.y < 0)
        return false;
    if(!m_buffers.at(slot.x).Exists(slot.y))
        return false;

    T value = m_buffers.at(slot.x).at(slot.y);

    if(m_buffers.at(slot.x).RemoveValueFromIndex(slot.y))
    {
        m_occupiedSlotsAmount--;
        m_valueToBufferIndex.erase(value);
        return true;
    }

    return false;
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
unsigned int NzSparseBufferSet<T>::GetBuffersAmount() const
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
/*

template <typename T>
NzVector2i NzSparseBufferSet<T>::UpdateValueKey(const T& key)
{
    NzVector2i location(-1,-1);

    return location;
}
*/
