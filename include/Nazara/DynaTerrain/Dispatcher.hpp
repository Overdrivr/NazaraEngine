// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DISPATCHER_HPP
#define NAZARA_DISPATCHER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/DynaTerrain/Zone.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <vector>
#include <queue>
#include <memory>

//FIX ME : Renommer en NzDynaTerrainMasterNode ?
class NAZARA_API NzDispatcher
{
    public:
        NzDispatcher(unsigned int patchBufferSize = 256);
        ~NzDispatcher();

        void DrawAll(bool viewFrustumCullingEnabled = true);

        unsigned int GetFreeBuffersAmount() const;

        bool Initialize(unsigned int zoneDepth, unsigned int bufferAmount);

        NzVertexBuffer* QueryFreeBuffer();

        bool RemovePatch(const NzTerrainNodeID& ID);
        void ReturnBuffer(NzVertexBuffer* buffer);

        bool SubmitPatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID);

        //FIX ME : Il n'est pas utile de réenvoyer toutes les vertices lors d'un update
        //Avec un index buffer 16 fois plus grand contenant les 16 configuration
        //il est possible en donnant un offset à la donnée d'utiliser un différent index de ses vertices
        bool UpdatePatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID);

    protected:
    private:
        bool m_isReady;
        unsigned int m_zoneDepth;
        unsigned int m_zonesAmountX;
        //Contient l'ensemble des zones
        std::vector<std::unique_ptr<NzZone>> m_zones;
        //Contient l'ensemble des buffers
            //Un buffer pèse 1 050 000 octets, il peut contenir 1750 patches de 25 vertices,(150 float | 600 octets), soit 262500 vertices
        std::vector<NzVertexBuffer*> m_buffers;
        //Contient les buffers libres
        std::queue<NzVertexBuffer*> m_freeBuffers;

        NzVertexElement m_elements[2];
        NzVertexDeclaration m_declaration;

        //NzIndexBuffer* m_indexBuffer;

        unsigned int m_bufferSize;
        unsigned int m_patchSize;
        unsigned int m_patchAmount;
};

#endif // NAZARA_DISPATCHER_HPP
