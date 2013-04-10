// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINMASTERNODE_HPP
#define NAZARA_TERRAINMASTERNODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/DynaTerrain/TerrainNode.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Math/BoundingBox.hpp>
#include <vector>
#include <queue>
#include <memory>

//FIX ME 2 : Utiliser un arbre en structure interne pour le fast culling
//FIX ME : Renommer m_zones, zoneDepth
//FIX ME : Gérer l'aabb
class NAZARA_API NzTerrainMasterNode// : public NzSceneNode
{
    public:
        NzTerrainMasterNode(unsigned int patchBufferSize = 256);
        ~NzTerrainMasterNode();

        //virtual void AddToRenderQueue(NzRenderQueue& renderQueue) const;

        virtual void Draw() const;

		//virtual const NzBoundingBoxf& GetBoundingBox() const;
		unsigned int GetFreeBuffersAmount() const;
		//virtual nzSceneNodeType GetSceneNodeType() const;


        bool Initialize(unsigned int zoneDepth);

        NzVertexBuffer* QueryFreeBuffer();

        bool RemovePatch(const NzTerrainNodeID& ID);
        void ReturnBuffer(NzVertexBuffer* buffer);

        //FIX ME : Renommer en AddMesh
        bool SubmitPatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID);

        //FIX ME : Il n'est pas utile de réenvoyer toutes les vertices lors d'un update
        //Avec un index buffer 16 fois plus grand contenant les 16 configuration
        //il est possible en donnant un offset à la donnée d'utiliser un différent index de ses vertices
        bool UpdatePatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID);

    protected:
        //virtual bool VisibilityTest(const NzFrustumf& frustum);

        //NzBoundingBoxf m_aabb;
    private:
        bool m_isReady;
        unsigned int m_zoneDepth;
        unsigned int m_zonesAmountX;

        std::vector<std::unique_ptr<NzTerrainNode>> m_zones;
        std::vector<NzVertexBuffer*> m_buffers;
        std::queue<NzVertexBuffer*> m_freeBuffers;

        NzVertexElement m_elements[2];
        NzVertexDeclaration m_declaration;

        unsigned int m_bufferSize;
        unsigned int m_patchSize;
        unsigned int m_patchAmount;
};

#endif // NAZARA_TERRAINMASTERNODE_HPP
