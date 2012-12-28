// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include "Zone.hpp"
#include "Enumerations.hpp"
#include <vector>
#include <queue>
#include <memory>

//FIX ME : Remplacer le vector par un quadtree

class NzDispatcher
{
    public:
        NzDispatcher();
        ~NzDispatcher();

        void DrawAll(bool viewFrustumCullingEnabled = true);

        unsigned int GetFreeBuffersAmount() const;

        bool Initialize(unsigned int zoneDepth, unsigned int bufferAmount);

        NzVertexBuffer* QueryFreeBuffer();

        void RemovePatch(const id& ID);
        void ReturnBuffer(NzVertexBuffer* buffer);

        bool SubmitPatch(const std::array<float,150>& subBuffer, const id& ID);

        bool UpdatePatch(const std::array<float,150>& subBuffer, const id& ID);
        void UpdateViewFrustum(/* */);

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

        std::unique_ptr<NzIndexBuffer> m_indexBuffer;
};

#endif // DISPATCHER_HPP
