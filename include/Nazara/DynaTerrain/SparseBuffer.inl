// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <iostream>

template <typename T>
NzSparseBuffer<T>::NzSparseBuffer(unsigned int bufferSize)
{
    m_bufferSize = bufferSize;
    m_occupiedSlotsAmount = 0;
    //Il y a m_bufferSize cases de libres à partir de l'index 0
    m_freeSlotBatches.push_front(NzVector2ui(0,bufferSize));
    //m_filledSlotBatches.push_front(NzVector2ui(0,0));
}

template <typename T>
NzSparseBuffer<T>::~NzSparseBuffer()
{
    //dtor
}

template <typename T>
int NzSparseBuffer<T>::FindKey(const T& key) const
{
    //On récupère l'emplacement de la valeur
    typename std::map<T,int>::const_iterator it = m_slots.find(key);

    //Si la valeur n'existe pas dans le buffer, il n'y a rien à supprimer
    if(it == m_slots.end())
        return -1;

    return (*it).second;
}

template <typename T>
unsigned int NzSparseBuffer<T>::GetFilledSlotsAmount() const
{
    return m_occupiedSlotsAmount;
}

template <typename T>
const std::list<NzVector2ui>& NzSparseBuffer<T>::GetFilledBatches() const
{
    return m_filledSlotBatches;
}

template <typename T>
std::list<NzVector2ui> NzSparseBuffer<T>::GetFilledBatchesCopy()
{
    return m_filledSlotBatches;
}

template <typename T>
const std::list<NzVector2ui>& NzSparseBuffer<T>::GetFreeBatches() const
{
    return m_freeSlotBatches;
}

template <typename T>
std::list<NzVector2ui> NzSparseBuffer<T>::GetFreeBatchesCopy()
{
    return m_freeSlotBatches;
}

template <typename T>
unsigned int NzSparseBuffer<T>::GetFreeSlotsAmount() const
{
    return m_bufferSize - m_occupiedSlotsAmount;
}

template <typename T>
int NzSparseBuffer<T>::InsertValueKey(const T& key)
{
    if(m_occupiedSlotsAmount == m_bufferSize)
    {
        std::cout<<"No empty slots"<<m_occupiedSlotsAmount<<std::endl;
        return -1;
    }


    //On récupère le premier emplacement libre avec m_freeSlotBatches
    unsigned int index = m_freeSlotBatches.front().Start();
    //On ajoute la valeur dans le buffer à l'emplacement libre
    m_slots[key] = index;

    //On met à jour les emplacements libres et pleins
        //-1 pour les libres
    NzBatch temp;
    if(m_freeSlotBatches.front().RemoveValue(index,temp) == 2)//Un batch a été splitté, on l'ajoute à la liste des vides et on en crée un dans la liste des pleins
    {
        m_freeSlotBatches.insert(m_freeSlotBatches.front()+1,temp);
        m_filledSlotBatches.push_back(NzBatch(index,1));
    }

        //+1 pour les pleins

    std::list<NzVector2ui>::iterator it_filled;

    //Si il n'y a pas d'espace plein disponible on en crée un et on lui affecte immédiatement la valeur
    if(m_filledSlotBatches.empty())
    {
        m_filledSlotBatches.push_front(NzBatch(index,1));
        it_filled = m_filledSlotBatches.begin();
    }
    else
    {

        bool done = false;
        //On l'ajoute à la liste des batches pleins
        for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
        {
            if((*it_filled).Add(index))//Insertion reussie
            {
                done = true;
                break;
            }
        }

        if(!done) //L'insertion a échoué, aucun emplacement libre n'a été trouvé, on en crée un nouveau
            m_filledSlotBatches.push_back(NzBatch(index,1));
    }

    //Si la case du tableau freeSlotBatches ne représente plus d'espace libre
    if(m_freeSlotBatches.front().IsEmpty())
    {

        //On vire la case concernée
        m_freeSlotBatches.pop_front();

        //On fusionne les deux blocs pleins consécutifs de filledSlotBatches
        ++it_filled;
        if(it_filled != m_filledSlotBatches.end())
        {
            --it_filled;
            if((*(it_filled).MergeWith(*(++it_filled)))
            {
                m_filledSlotBatches.erase(it_filled);
            }
        }
    }

    m_occupiedSlotsAmount++;

    return index;
}

template <typename T>
NzVector2i NzSparseBuffer<T>::ReduceFragmentation()
{
    return NzVector2i(0,0);
}

template <typename T>
int NzSparseBuffer<T>::RemoveValueKey(const T& key)
{
    //On récupère l'emplacement de la valeur
    typename std::map<T,int>::iterator it = m_slots.find(key);

    //Si la valeur n'existe pas dans le buffer, il n'y a rien à supprimer
    if(it == m_slots.end())
        return -1;

    unsigned int index = (*it).second;
    //On identifie la situation, il y a 5 cas possibles. La suppression peut avoir lieu
        //1)a l'index 0
        //2)au debut d'un lot plein quelconque (index > 0)
        //3)au milieu d'un lot plein
        //4)a la fin d'un lot plein
        //5)au dernier index (m_bufferSize-1)


    //si l'emplacement libéré ne contenait qu'un seul slot occupé, il faut fusionner deux lots consécutifs dans m_freeSlotBatches

    std::list<NzBatch>::iterator it_free;

    //Pour savoir dans lequel des cas on se trouve
    unsigned int situation = 0;

    NzBatch temp;
    std::list<NzVector2ui>::iterator it_filled;

    //Mise à jour des lots pleins
    for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
    {
        situation = (*it_filled).Remove(index,temp);

        if(situation == 2)
        {
            //Un split a eu lieu, on rajoute le batch plein dans la liste des pleins et on en crée un nouveau dans la vide
            ++it_filled;
            m_filledSlotBatches.Insert(it_filled,temp);
            --it_filled;
            m_freeSlotBatches.push_back(NzBatch(index,1));
            break;
        }
        else if(situation == 1)
        {
            //La suppression s'est faite sans encombres
            break;
        }
    }

/*

    std::cout<<"situation"<<situation<<std::endl;
    bool treated = false;
    //Si il n'y a aucun lot vide, on en crée un et on lui affecte immédiatement la valeur
    if(m_freeSlotBatches.empty())
    {
        m_freeSlotBatches.push_front(NzVector2ui(index,1));
        it_free = m_freeSlotBatches.begin();
        treated = true;
    }
    else
    {
        //Mise à jour des lots vides
        for(it_free = m_freeSlotBatches.begin() ; it_free != m_freeSlotBatches.end() ; ++it_free)
        {

            if((*it_free).x + (*it_free).y == index)//Si il peut être ajouté en fin d'un lot vide existant
            {
                (*it_free).y++;//Un slot libre supplémentaire
                m_slots.erase(key);//On retire la valeur
                treated = true;
                break;
            }
            else if((*it_free).x - 1 == index)//Si il peut être ajouté en début d'un lot vide existant
            {
                (*it_free).x--;//On recule l'index de 1 pour intégrer le nouvel emplacement
                (*it_free).y++;//Idem qu'avant
                m_slots.erase(key);
                treated = true;
                break;
            }
            else if((*it_free).x > index) //Si il faut créer un nouveau lot dans freeSlotBatches
            {
                m_freeSlotBatches.insert(it_free,NzVector2ui(index,1));
                m_slots.erase(key);
                treated = true;
                break;
            }
        }

    }

    if(!treated && situation == 5)//Il est impossible de dépasser l'index de la dernière case, il faut traiter ce cas à part
    {
        it_free++;
        m_freeSlotBatches.insert(it_free,NzVector2ui(index,1));
        m_slots.erase(key);
        treated = true;
    }

    if(!treated && situation == 3)
    {
        it_free++;
        m_freeSlotBatches.insert(it_free,NzVector2ui(index,1));
        m_slots.erase(key);
        treated = true;

    }

    if(!treated)
    {
        std::cout<<"Nothing done with free"<<std::endl;
    }
    if(situation == 0)
    {
        std::cout<<"SparseBuffer::Remove : "<<index<<" : Something went wrong"<<std::endl;
        return -1;
    }

    if(situation == 2 || situation == 4)
    {
        if((*it_filled).y <= 1)//Si le lot ne contenait qu'un slot plein, il est désormais vide, on le supprime
        {
            //On supprime le lot plein désormais vide
            m_filledSlotBatches.erase(it_filled);
            //On doit fusionner avec l'emplacement suivant
            unsigned int offset = (*it_free).y;
            it_free = m_freeSlotBatches.erase(it_free);
            --it_free;
            (*it_free).y += offset;
        }
    }*/

    if((*it_filled).IsEmpty())
    {
        //On vire la case concernée
        m_filledSlotBatches.erase(it_filled);

        //On fusionne les deux blocs pleins consécutifs de freeSlotBatches
        ++it_free;
        if(it_free != m_freeSlotBatches.end())
        {
            --it_free;
            if((*(it_free).MergeWith(*(++it_free)))
            {
                m_freeSlotBatches.erase(it_free);
            }
        }
    }

    m_occupiedSlotsAmount--;
    return index;
}
