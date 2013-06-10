// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

template <typename T>
NzIntervalBuffer<T>::NzIntervalBuffer(unsigned int bufferSize)
{
    m_bufferSize = bufferSize;
    m_occupiedSlotsAmount = 0;
    //Il y a m_bufferSize cases de libres à partir de l'index 0
    m_freeSlotBatches.push_front(NzBatch(0,bufferSize));
}

template <typename T>
NzIntervalBuffer<T>::~NzIntervalBuffer()
{
    //dtor
}

template <typename T>
int NzIntervalBuffer<T>::FindValue(const T& value) const
{
    //On récupère l'emplacement de la valeur
    typename std::map<T,int>::const_iterator it = m_slots.find(key);

    //Si la valeur n'existe pas dans le buffer, il n'y a rien à supprimer
    if(it == m_slots.end())
        return -1;

    return (*it).second;
}

template <typename T>
unsigned int NzIntervalBuffer<T>::GetFilledSlotsAmount() const
{
    return m_occupiedSlotsAmount;
}

template <typename T>
const std::list<NzBatch>& NzIntervalBuffer<T>::GetFilledIntervals() const
{
    return m_filledSlotBatches;
}

template <typename T>
std::list<NzBatch> NzIntervalBuffer<T>::GetFilledIntervalsCopy()
{
    return m_filledSlotBatches;
}

template <typename T>
const std::list<NzBatch>& NzIntervalBuffer<T>::GetFreeIntervals() const
{
    return m_freeSlotBatches;
}

template <typename T>
std::list<NzBatch> NzIntervalBuffer<T>::GetFreeIntervalsCopy()
{
    return m_freeSlotBatches;
}

template <typename T>
unsigned int NzIntervalBuffer<T>::GetFreeSlotsAmount() const
{
    return m_bufferSize - m_occupiedSlotsAmount;
}

template <typename T>
int NzIntervalBuffer<T>::InsertValue(const T& value)
{
    //Pas d'espace libre
    if(m_occupiedSlotsAmount == m_bufferSize)
        return -1;

    //On récupère le premier emplacement libre avec m_freeSlotBatches
    unsigned int index = m_freeSlotBatches.front().Start();

    //On ajoute la valeur dans le buffer à l'emplacement libre
    m_slots[key] = index;

    if(!AtomicKeyRemoval(m_freeSlotBatches,index))
        return -1;

    //L'insertion ne peut pas échouer
    AtomicKeyInsertion(m_filledSlotBatches,index);

    m_occupiedSlotsAmount++;

    return index;
}

template <typename T>
NzVector2i NzIntervalBuffer<T>::ReduceFragmentation()
{
    return NzVector2i(0,0);
}

template <typename T>
int NzIntervalBuffer<T>::RemoveValue(const T& value)
{
    //On récupère l'emplacement de la valeur
    typename std::map<T,int>::iterator it = m_slots.find(key);

    //Si la valeur n'existe pas dans le buffer, il n'y a rien à supprimer
    if(it == m_slots.end())
        return -1;

    unsigned int index = it->second;

    if(!RemoveValueFromSingleBuffer(m_filledSlotBatches,index))
        return -1;

    //L'insertion ne peut pas échouer
    InsertValueToSingleBuffer(m_freeSlotBatches,index);

    m_occupiedSlotsAmount--;

    return index;
}

template <typename T>
bool NzSparseBuffer<T>::InsertValueToSingleBuffer(std::list<NzBatch>& buffer, unsigned int index)
{
    std::list<NzBatch>::iterator it;
    std::list<NzBatch>::iterator it_2;
    std::list<NzBatch>::iterator it_last = buffer.end();
    it_last--;

    //Si il n'y a pas d'espace disponible on en crée un et on lui affecte immédiatement la valeur
    if(buffer.empty())
    {
        buffer.push_front(NzBatch(index,1));
        it = buffer.begin();
    }
    else
    {
        //On l'ajoute à la liste des batches pleins
        for(it = buffer.begin() ; it != buffer.end() ; ++it)
        {
            if(it->Add(index))//Ajout à un emplacement existant réussi
            {
                break;
            }
            else if(it->Start() > index + 1)//Création d'un nouveau bloc nécessaire
            {
                buffer.insert(it,NzBatch(index,1));
                break;
            }
            else if(it == it_last)//L'index est situé après le dernier emplacement, création d'un nouveau bloc a la fin
            {
                buffer.push_back(NzBatch(index,1));
                break;
            }
        }

    }

    //L'insertion a été réalisée, maintenant on regarde si un regroupement (avant et/ou après) est possible

    //avant
    if(it != buffer.begin())
    {
        it--;
        it_2 = it;
        it++;

        if(it_2->MergeWith(*it))
        {
            it = buffer.erase(it);
        }
    }

    //après
    it++;
    if(it != buffer.end())
    {
        it_2 = it;
        it--;

        if(it->MergeWith(*it_2))
        {
            it = buffer.erase(it_2);
        }
    }

    return true;
}

template <typename T>
bool NzSparseBuffer<T>::RemoveValueFromSingleBuffer(std::list<NzBatch>& buffer, unsigned int index)
{
    std::list<NzBatch>::iterator it;
    std::list<NzBatch>::iterator it_2;
    std::list<NzBatch>::iterator it_last = buffer.end();
    it_last--;

    //Si il n'y a pas d'emplacements occupés, aucune suppression à effectuer
    if(buffer.empty())
    {
        return false;
    }
    else
    {
        int situation;
        NzBatch temp(0,0);

        //On l'ajoute à la liste des batches pleins
        for(it = buffer.begin() ; it != buffer.end() ; ++it)
        {
            situation = it->Remove(index,temp);

            if(situation == 2)//Une suppression au milieu a eu lieu, un nouvel emplacement a été créé
            {
                ++it;
                buffer.insert(it,temp);
                --it;
                break;
            }
            else if(situation == 1)
            {
                //La suppression s'est faite sans encombres
                break;
            }
            else if(situation == 0)
            {
                //Le bloc est vide, on le supprime
                it = buffer.erase(it);
                break;
            }
        }

    }

    return true;
}
