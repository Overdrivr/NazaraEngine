// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

template <typename T>
NzSparseBufferSet<T>::NzSparseBufferSet()
{
    //ctor
}

template <typename T>
NzSparseBufferSet<T>::~NzSparseBufferSet()
{
    //dtor
}

template <typename T>
void NzSparseBufferSet<T>::AddBuffer()
{

}

template <typename T>
nzBufferLocation NzSparseBufferSet<T>::FindValue(const T& value) const
{

}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetBufferAmount() const
{

}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetFreeBuffersAmount() const
{

}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetTotalFreeSlotsAmount() const
{

}

template <typename T>
unsigned int NzSparseBufferSet<T>::GetTotalOccupiedSlotsAmount() const
{

}

template <typename T>
nzBufferLocation NzSparseBufferSet<T>::InsertValue(const T& value)
{

}

template <typename T>
NzVector2<nzBufferLocation> NzSparseBufferSet<T>::ReduceFragmentation()
{

}

template <typename T>
bool NzSparseBufferSet<T>::RemoveBuffer(unsigned int index)
{

}

template <typename T>
nzBufferLocation NzSparseBufferSet<T>::RemoveValue(const T& value)
{

}
