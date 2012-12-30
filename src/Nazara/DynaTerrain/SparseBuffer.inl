// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

template <typename T>
NzSparseBuffer<T>::NzSparseBuffer(unsigned int bufferSize)
{
    m_bufferSize = bufferSize;
    //Il y a m_bufferSize cases de libres � partir de l'index 0
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

    //On r�cup�re le premier emplacement libre avec m_freeSlotBatches
    unsigned int index = m_freeSlotBatches.front().x;
    //On ajoute la valeur dans le buffer � l'emplacement libre
    m_slots[value] = index;
    m_internalBuffer.at(index) = value;

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
    //On r�cup�re l'emplacement de la valeur
    unsigned int index = m_slots[value];

    //On localise le nouvel emplacement libre cr��
        //Soit il peut �tre ajout� en d�but ou fin d'un lot d�j� existant
        //Soit il faut en cr�er un nouveau
    //si l'emplacement lib�r� ne contenait qu'un seul slot occup�, il faut fusionner deux lots dans m_freeSlotBatches

    std::list<NzVector2ui>::iterator it_free;

    for(it_free = m_freeSlotBatches.begin() ; it_free != m_freeSlotBatches.end() ; ++it_free)
    {
        if((*it_free).x + (*it_free).y == index)//Si il peut �tre ajout� en fin d'un lot existant
        {

        }
        else if((*it_free).x - 1 == index)//Si il peut �tre ajout� en d�but d'un lot existant
        {

        }
        else if((*it_free).x > index) //On vient de d�passer l'index de 2, il faut cr�er un nouveau lot dans freeSlotBatches
        {

            break;
        }
    }
}
