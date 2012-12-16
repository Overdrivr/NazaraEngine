// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

template <typename T>
NzSparseBuffer<T>::NzSparseBuffer(unsigned int bufferSize)
{
    m_bufferSize = bufferSize;
}

template <typename T>
NzSparseBuffer<T>::~NzSparseBuffer()
{
    //dtor
}

template <typename T>
unsigned int NzSparseBuffer<T>::GetFreeSlotsAmount() const
{
    return 0;
}

template <typename T>
int NzSparseBuffer<T>::InsertValue(const T& value)
{/*
        //! Les indices entre le sparsebuffer et le vertexbuffer sont décalés de +1 dans le sparsebuffer
    std::list<xid>::iterator it = m_sparseBuffers.at(loc.buffer).begin();

    for(int i(0) ; i < loc.index ; ++i)
        it++;

    //Si la case précédente indique que la case suivante est libre
    if((*it).freeAdjacentConsecutiveSlots > 0)
    {
        //La case précédente transmet son nombre de cases adjacentes libres (- 1) à sa nouvelle case suivante
        unsigned int tempFreeSlots = (*it).freeAdjacentConsecutiveSlots;

        //On peut insérer une case à l'emplacement suivant
        it++;
        xid value;
        value.sx = ID.sx;
        value.sy = ID.sy;
        value.lvl = ID.lvl;
        value.freeAdjacentConsecutiveSlots = tempFreeSlots - 1;
        std::cout<<"val "<<value.freeAdjacentConsecutiveSlots<<std::endl;
        m_sparseBuffers.at(loc.buffer).insert(it,value);

        return true;
    }
    else
        return false;
*/

    return 0;
}

template <typename T>
NzVector2i NzSparseBuffer<T>::ReduceFragmentation()
{
    return NzVector2i(0,0);
}

template <typename T>
bool NzSparseBuffer<T>::RemoveValue(const T& value)
{
    return false;
}
