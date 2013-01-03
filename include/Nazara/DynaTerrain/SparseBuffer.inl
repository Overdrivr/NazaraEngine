// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <iostream>

template <typename T>
NzSparseBuffer<T>::NzSparseBuffer(unsigned int bufferSize)
{
    m_bufferSize = bufferSize;
    m_occupiedSlotsAmount = 0;
    //Il y a m_bufferSize cases de libres � partir de l'index 0
    m_freeSlotBatches.push_front(NzVector2ui(0,bufferSize));
    m_filledSlotBatches.push_front(NzVector2ui(0,0));
}

template <typename T>
NzSparseBuffer<T>::~NzSparseBuffer()
{
    //dtor
}

template <typename T>
int NzSparseBuffer<T>::FindValue(const T& value)
{
    //On r�cup�re l'emplacement de la valeur
    typename std::map<T,int>::iterator it = m_slots.find(value);

    //Si la valeur n'existe pas dans le buffer, il n'y a rien � supprimer
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
const std::list<NzVector2ui>& NzSparseBuffer<T>::GetFilledSlotBatches()
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

    //On r�cup�re le premier emplacement libre avec m_freeSlotBatches
    unsigned int index = m_freeSlotBatches.front().x;
    //On ajoute la valeur dans le buffer � l'emplacement libre
    m_slots[value] = index;
    //m_internalBuffer.at(index) = value;

    //On met � jour les emplacements libres et pleins
        //-1 pour les libres
    unsigned int freeConsecutiveSlots = m_freeSlotBatches.front().y - 1;
        //+1 pour les pleins
            //On localise le bon emplacement des pleins
    std::list<NzVector2ui>::iterator it_filled;
                //L'emplacement d�sormais occup� sera ajout� en fin d'un bloc de vertices existant
                //(*it).x = index2
                //(*it).y = offset
    for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
    {
        if((*it_filled).x > index) //On vient de d�passer l'index, il faut cr�er une nouvelle case (normalement non appel�)
        {
            std::cout<<"SparseBuffer::InsertValue problem"<<std::endl;
            break;
        }
        else if((*it_filled).x + (*it_filled).y == index)
        {
            //L'iterateur est bien plac�
            //+1 comme pr�vu
            (*it_filled).y++;
            break;
        }
    }

    //Si la case du tableau freeSlotBatches ne repr�sente plus d'espace libre
    if(freeConsecutiveSlots == 0)
    {
        //On vire la case concern�e
        m_freeSlotBatches.pop_front();
        //On fusionne les deux blocs pleins cons�cutifs de filledSlotBatches
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
int NzSparseBuffer<T>::RemoveValue(const T& value)
{
    //On r�cup�re l'emplacement de la valeur
    typename std::map<T,int>::iterator it = m_slots.find(value);

    //Si la valeur n'existe pas dans le buffer, il n'y a rien � supprimer
    if(it == m_slots.end())
        return -1;

    unsigned int index = (*it).second;

    //On localise le nouvel emplacement libre cr��
        //Soit il peut �tre ajout� en d�but ou fin d'un lot d�j� existant
        //Soit la suppression a lieu au milieu d'un lot, il faut cr�er un nouveau plein et vide
    //si l'emplacement lib�r� ne contenait qu'un seul slot occup�, il faut fusionner deux lots cons�cutifs dans m_freeSlotBatches

    std::list<NzVector2ui>::iterator it_free;

    //Pour savoir dans lequel des cas on se trouve
    unsigned int situation = 0;

    for(it_free = m_freeSlotBatches.begin() ; it_free != m_freeSlotBatches.end() ; ++it_free)
    {
        if((*it_free).x + (*it_free).y == index)//Si il peut �tre ajout� en fin d'un lot existant
        {
            (*it_free).y++;//Un slot libre suppl�mentaire
            m_slots.erase(value);//On retire la valeur
            m_occupiedSlotsAmount--;//Et on diminue de 1 le compteur
            situation = 1;
            break;
        }
        else if((*it_free).x - 1 == index)//Si il peut �tre ajout� en d�but d'un lot existant
        {
            (*it_free).x--;//On recule l'index de 1 pour int�grer le nouvel emplacement
            (*it_free).y++;//Idem qu'avant
            m_slots.erase(value);
            m_occupiedSlotsAmount--;
            situation = 2;
            break;
        }
        else if((*it_free).x > index) //On vient de d�passer l'index de 2, il faut cr�er un nouveau lot dans freeSlotBatches
        {
            m_freeSlotBatches.insert(it_free,NzVector2ui(index,1));
            m_slots.erase(value);
            m_occupiedSlotsAmount--;
            situation = 3;
            break;
        }
    }

    if(situation == 0)
    {
        std::cout<<"SparseBuffer::Remove : Something went wrong with freeBatches"<<std::endl;
        return -1;
    }

    situation = 0;
    //On localise l'emplacement plein supprim�

    std::list<NzVector2ui>::iterator it_filled;

    for(it_filled = m_filledSlotBatches.begin() ; it_filled != m_filledSlotBatches.end() ; ++it_filled)
    {
        if((*it_filled).x == index)//Normalement on est dans la situation 1
        {
            (*it_filled).x++;//On d�cale le premier index
            situation = 1;
            break;
        }
        else if((*it_filled).x + (*it_filled).y - 1 == index)//Normalement situation 2
        {
            situation = 2;
            break;
        }
        else if((*it_filled).x < index && (*it_filled).x + (*it_filled).y > index) //Normalement 3
        {
            //Taille du nouveau lot � cr��
            unsigned int offset = (*it_filled).y - (index - (*it_filled).x) - 1;
            //Nouvelle taille du premier lot
            (*it_filled).y = index - (*it_filled).x;
            //L'insertion dans une liste a lieu a la position pr�c�dant l'iterateur, on avance de 1 pour �tre bien plac�
            it_filled++;
            //On ins�re un nouveau lot
            m_filledSlotBatches.insert(it_filled,NzVector2ui(index+1,offset));
            situation = 3;
            break;
        }
    }

    if(situation == 0)
    {
        std::cout<<"SparseBuffer::Remove : Something went wrong with filledBatches"<<std::endl;
        return -1;
    }

    if(situation == 1 || situation == 2)
    {
        //Dans la situation 3, le nouvel emplacement est cr�� au milieu d'un lot, pas au d�but ni � la fin
        //Le lot a donc une taille de 3, jamais 1, et la taille a d�j� �t� mise � jour

        if((*it_filled).y <= 1)//Si le lot ne contenait qu'un slot plein, il est d�sormais vide, on le supprime
        {
            m_filledSlotBatches.erase(it_filled);
            //On fusionne les 2 blocs vides cons�cutifs
            ++it_free;
            unsigned int offset = (*it_free).y;
            it_free = m_freeSlotBatches.erase(it_free);
            --it_free;
            (*it_free).y += offset;
        }
        else
            (*it_filled).y--;
    }


    return index;
}
