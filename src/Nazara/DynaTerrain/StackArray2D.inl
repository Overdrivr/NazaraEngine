// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <iostream>
#include <Nazara/Core/Debug.hpp>

template <typename T>
StackArray2D<T>::StackArray2D()
{
    m_level = 0;
    m_sizex = 0;
    m_sizey = 0;
}

template <typename T>
const T& StackArray2D<T>::at(const id& ID) const
{
    typename std::map<id, T>::iterator it = m_data.find(ID);

    if(it != m_data.end())
        return it->second;
    else
        std::cout<<"StackArray2D out of range ("<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<")"<<std::endl;
}

template <typename T>
const T& StackArray2D<T>::at(unsigned int level, unsigned int i, unsigned int j) const
{
    typename std::map<id, T>::iterator it = m_data.find(id(level,i,j));

    if(it != m_data.end())
        return it->second;
    else
        std::cout<<"StackArray2D out of range ("<<level<<"|"<<i<<"|"<<j<<")"<<std::endl;
}

template <typename T>
T& StackArray2D<T>::at(const id& ID)
{
    return m_data[ID];
}

template <typename T>
T& StackArray2D<T>::at(unsigned int level, unsigned int i, unsigned int j)
{
    return m_data[id(level,i,j)];
}

template <typename T>
int StackArray2D<T>::Erase(const id& ID)
{
    return m_data.erase(ID);
}

template <typename T>
int StackArray2D<T>::Erase(unsigned int level, unsigned int i, unsigned int j)
{
    return m_data.erase(id(level,i,j));
}

template <typename T>
bool StackArray2D<T>::Exists(const id& ID) const
{
    typename std::map<id, T>::const_iterator it = m_data.find(ID);

    if(it != m_data.end())
        return true;
    else
        return false;
}

template <typename T>
bool StackArray2D<T>::Exists(unsigned int level, unsigned int i, unsigned int j) const
{
    typename std::map<id, T>::const_iterator it = m_data.find(id(level,i,j));

    if(it != m_data.end())
        return true;
    else
        return false;
}

template <typename T>
void StackArray2D<T>::Display()
{
    typename std::map<id,T>::iterator it;
    for(it = m_data.begin() ; it != m_data.end(); it++)
        std::cout << ((*it).first).lvl <<"|"<< ((*it).first).sx<<"|"<<((*it).first).sy<< std::endl;
}


#include <Nazara/Core/DebugOff.hpp>
