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
    m_filledSlotBatches.push_front(NzVector2ui(0,0));
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
    if(m_freeSlotBatches.empty())
    {
        std::cout<<"SparseBuffer::InsertValueKey : freeSlotBatches empty : management issues"<<std::endl;
        return -1;
    }

    if(m_occupiedSlotsAmount == m_bufferSize)
    {
        //std::cout<<"No empty slots"<<std::endl;
        return -1;
    }


    //On récupère le premier emplacement libre avec m_freeSlotBatches
    unsigned int index = m_freeSlotBatches.front().x;
    //On ajoute la valeur dans le buffer à l'emplacement libre
    m_slots[key] = index;
    //std::cout<<"Supplied index "<<index<<std::endl;
    //m_internalBuffer.at(index) = value;

    //On met à jour les emplacements libres et pleins
        //-1 pour les libres
    unsigned int freeConsecutiveSlots = m_freeSlotBatches.front().y--;
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
        //On vire la case concernée si elle n'est pas la dernière
        if(m_freeSlotBatches.size() > 1)
            m_freeSlotBatches.pop_front();

        //On fusionne les deux blocs pleins consécutifs de filledSlotBatches
        ++it_filled;
        unsigned int offset = (*it_filled).y;
        it_filled = m_filledSlotBatches.erase(it_filled);
        --it_filled;
        (*it_filled).y += offset;
    }
    else
    {
        //On avance simplement de 1 le premier index de libre
        m_freeSlotBatches.front().x++;
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

    std::list<NzVector2ui>::iterator it_free;

    //Pour savoir dans lequel des cas on se trouve
    unsigned int situation = 0;


    std::list<NzVector2ui>::iterator it_filled;
    //Recherche de la situation & mise à jour des lots pleins
    for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
    {
        if((*it_filled).x == index)//Situation 1 ou 2
        {
            (*it_filled).x++;//On décale le premier index
            (*it_filled).y--;

            if(index == 0)
                situation = 1;
            else
                situation = 2;

            break;
        }
        else if((*it_filled).x + (*it_filled).y - 1 == index)//Normalement situation 4 ou 5
        {
            (*it_filled).y--;

            if(index == m_bufferSize-1)
                situation = 5;
            else
                situation = 4;

            break;
        }
        else if((*it_filled).x < index && (*it_filled).x + (*it_filled).y > index) //Normalement 3
        {
            //Taille du nouveau lot à créé
            unsigned int offset = (*it_filled).y - (index - (*it_filled).x) - 1;
            //Nouvelle taille du premier lot
            (*it_filled).y = index - (*it_filled).x;
            //L'insertion dans une liste a lieu a la position précédant l'iterateur, on avance de 1 pour être bien placé
            it_filled++;
            //On insère un nouveau lot
            m_filledSlotBatches.insert(it_filled,NzVector2ui(index+1,offset));
            situation = 3;
            break;
        }
    }
    std::cout<<"situation"<<situation<<std::endl;

    //mise à jour des lots vides

    /*

    for(it_free = m_freeSlotBatches.begin() ; it_free != m_freeSlotBatches.end() ; ++it_free)
    {
        if((*it_free).x + (*it_free).y == index)//Si il peut être ajouté en fin d'un lot vide existant
        {
            (*it_free).y++;//Un slot libre supplémentaire
            m_slots.erase(key);//On retire la valeur
            m_occupiedSlotsAmount--;//Et on diminue de 1 le compteur



            break;
        }
        else if((*it_free).x - 1 == index)//Si il peut être ajouté en début d'un lot vide existant
        {
            (*it_free).x--;//On recule l'index de 1 pour intégrer le nouvel emplacement
            (*it_free).y++;//Idem qu'avant
            m_slots.erase(key);
            m_occupiedSlotsAmount--;



            break;
        }
        else if((*it_free).x > index) //On vient de dépasser l'index de 2, il faut créer un nouveau lot dans freeSlotBatches
        {
            m_freeSlotBatches.insert(it_free,NzVector2ui(index,1));
            m_slots.erase(key);
            m_occupiedSlotsAmount--;
            situation = 3;
            break;
        }
    }

    std::cout<<"situation "<<situation<<std::endl;

    if(situation == 0)
    {
        std::cout<<"SparseBuffer::Remove : "<<index<<" : Something went wrong with freeBatches"<<std::endl;
        return -1;
    }

    //On localise l'emplacement plein supprimé

    std::list<NzVector2ui>::iterator it_filled;

    for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
    {
        if((*it_filled).x == index)//Normalement on est dans la situation 1 ou 2
        {
            (*it_filled).x++;//On décale le premier index
            (*it_filled).y--;
            //situation = 1;
            break;
        }
        else if((*it_filled).x + (*it_filled).y - 1 == index)//Normalement situation 4 ou 5
        {
            (*it_filled).y--;
            //situation = 2;
            break;
        }
        else if((*it_filled).x < index && (*it_filled).x + (*it_filled).y > index) //Normalement 3
        {
            //Taille du nouveau lot à créé
            unsigned int offset = (*it_filled).y - (index - (*it_filled).x) - 1;
            //Nouvelle taille du premier lot
            (*it_filled).y = index - (*it_filled).x;
            //L'insertion dans une liste a lieu a la position précédant l'iterateur, on avance de 1 pour être bien placé
            it_filled++;
            //On insère un nouveau lot
            m_filledSlotBatches.insert(it_filled,NzVector2ui(index+1,offset));
            //situation = 3;
            break;
        }
    }
    std::cout<<"situation bis"<<situation<<std::endl;

    if(situation == 0)
    {
        std::cout<<"SparseBuffer::Remove : "<<index<<" : Something went wrong with filledBatches"<<std::endl;
        return -1;
    }

    if(situation == 2 || situation == 4)
    {
        //Dans la situation 3, le nouvel emplacement est créé au milieu d'un lot, pas au début ni à la fin
        //Le lot a donc une taille de 3, jamais 1, et la taille a déjà été mise à jour

        if((*it_filled).y <= 1)//Si le lot ne contenait qu'un slot plein, il est désormais vide, on le supprime
        {
            //On supprime le lot plein désormais vide
            m_filledSlotBatches.erase(it_filled);
            std::cout<<"lib"<<std::endl;

            if(situation == 4)
            {
                std::cout<<"here"<<std::endl;
                //On doit fusionner avec l'emplacement suivant
                unsigned int offset = (*it_free).y;
                it_free = m_freeSlotBatches.erase(it_free);
                --it_free;
                (*it_free).y += offset;
            }
            else if(situation == 2)
            {
                std::cout<<"here2"<<std::endl;
                //On doit fusionner avec l'emplacement precedent
                unsigned int offset = (*it_free).y;
                it_free = m_freeSlotBatches.erase(it_free);
                --it_free;
                (*it_free).y += offset;
            }


        }
    }*/


    return index;
}
