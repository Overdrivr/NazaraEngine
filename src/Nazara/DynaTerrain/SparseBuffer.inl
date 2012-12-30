// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

template <typename T>
NzSparseBuffer<T>::NzSparseBuffer(unsigned int bufferSize)
{
    m_bufferSize = bufferSize;
    //Il y a m_bufferSize cases de libres à partir de l'index 0
    m_freeSlotBatches.push_front(NzVector2ui(0,bufferSize));
}

template <typename T>
NzSparseBuffer<T>::~NzSparseBuffer()
{
    //dtor
}

template <typename T>
const std::list<NzVector2ui>& NzSparseBuffer<T>::GetVerticeIndexBatches()
{
    return m_filledSlotBatches;
}

template <typename T>
unsigned int NzSparseBuffer<T>::GetFreeSlotsAmount() const
{
    return m_bufferSize - m_occupiedSlotsAmount;
}

template <typename T>
int NzSparseBuffer<T>::InsertValue(const T& value)
{
    if(m_freeSlotBatches.empty())
        return -1;

    //On récupère le premier emplacement libre avec m_freeSlotBatches
    unsigned int index = m_freeSlotBatches.front().x;
    //On ajoute la valeur dans le buffer à l'emplacement libre
    m_slots[value] = index;
    m_internalBuffer.at(index) = value;

    //On met à jour les emplacements libres et pleins
        //-1 pour les libres
    unsigned int freeConsecutiveSlots = m_freeSlotBatches.front().y - 1;
        //+1 pour les pleins
            //On localise le bon emplacement des pleins
    std::list<NzVector2ui>::iterator it_filled;
                //L'emplacement désormais occupé sera ajouté en fin d'un bloc de vertices existant
                //(*it).x = index2
                //(*it).y = offset
    for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
    {
        if((*it_filled).x > index) //On vient de dépasser l'index, il faut créer une nouvelle case (normalement non appelé)
        {
            std::cout<<"SparseBuffer::InsertValue problem"<<std::endl;
            break;
        }
        else if((*it_filled).x + (*it_filled).y == index)
        {
            //L'iterateur est bien placé
            //+1 comme prévu
            (*it_filled).y++;
            break;
        }
    }

    //Si la case du tableau freeSlotBatches ne représente plus d'espace libre
    if(freeConsecutiveSlots == 0)
    {
        //On vire la case concernée
        m_freeSlotBatches.pop_front();
        //On fusionne les deux blocs pleins consécutifs de filledSlotBatches
        (*it_filled).y += (*(it_filled++)).y;
    }
    else
    {
        //On avance simplement de 1 le premier index de libre
        m_freeSlotBatches.front().x++;
    }

    return index;
}

template <typename T>
NzVector2i NzSparseBuffer<T>::ReduceFragmentation()
{
    return NzVector2i(0,0);
}

template <typename T>
bool NzSparseBuffer<T>::RemoveValue(const T& value)
{
    //On récupère l'emplacement de la valeur
    unsigned int index = m_slots[value];

    //On localise le nouvel emplacement libre créé
        //Soit il peut être ajouté en début ou fin d'un lot déjà existant
        //Soit il faut en créer un nouveau
    //si l'emplacement libéré ne contenait qu'un seul slot occupé, il faut fusionner deux lots dans m_freeSlotBatches

    std::list<NzVector2ui>::iterator it_free;

    for(it_free = m_freeSlotBatches.begin() ; it_free != m_freeSlotBatches.end() ; ++it_free)
    {
        if((*it_free).x + (*it_free).y == index)//Si il peut être ajouté en fin d'un lot existant
        {

        }
        else if((*it_free).x - 1 == index)//Si il peut être ajouté en début d'un lot existant
        {

        }
        else if((*it_free).x > index) //On vient de dépasser l'index de 2, il faut créer un nouveau lot dans freeSlotBatches
        {

            break;
        }
    }
}
