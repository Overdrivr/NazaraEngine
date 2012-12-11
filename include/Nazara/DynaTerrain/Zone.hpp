// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef ZONE_HPP
#define ZONE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include "Enumerations.hpp"
#include <array>
#include <vector>
#include <deque>
#include <map>
#include <queue>

class NzZone
{
    public:
        NzZone();
        ~NzZone();
        void AddBuffer(NzVertexBuffer* buffer);
        void AddPatch(const std::array<float,150>& vertices, const id& ID);
        void DrawBuffers();
        unsigned int GetFreeBuffersAmount();
        unsigned int GetFreeSubBuffersAmount();
        bool RemoveFreeBuffer(NzVertexBuffer* buffer);
        bool RemovePatch(const id& ID);
        bool UpdatePatch(const std::array<float,150>& vertices, const id& ID);
    protected:
    private:
        //Les buffers allou�s � la zone
            //Ces buffers sont ceux envoy�s au GPU pour affichage
        std::vector<NzVertexBuffer*> m_buffers;

        //Index des emplacements libres
            //Lors de l'enregistrement d'un nouveau patch, celui-ci doit prendre l'emplacement au d�but de la file
            //L'emplacement m�moire associ� est alors situ� � m_buffers.at(m_freeSpots.front()*bufferSize)
            //bufferSize vaut 150 (25 points : position + normale)
        std::deque<nzBufferLocation> m_freeSpotsIndex;

        //Contient l'ensemble des id des patchs contenus par la zone et leur position dans le buffer
            //tout patch contenu par la zone est localisable grace � ce conteneur.
            //Le buffer (de 25 points) du patch est alors situ� � m_buffers.at(m_meshesIndexIterator->second())
        std::map<id,unsigned int> m_patchesIndex;

        //Contient l'ensemble des patches qui n'ont pas pu �tre mis en m�moire vid�o
            //pour cause d'espace insuffisant
        std::queue<float> m_unbufferedPatches;

        unsigned int m_patchAmount;
        unsigned int m_bufferAmount;
};

#endif // ZONE_HPP
