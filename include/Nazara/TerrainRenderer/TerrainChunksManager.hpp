// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Terrain Renderer module".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINCHUNKSMANAGER_HPP
#define NAZARA_TERRAINCHUNKSMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
//#include <Nazara/DynaTerrain/TerrainNode.hpp>
#include <Nazara/DynaTerrain/Enums.hpp>
//#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/TerrainRenderer/TerrainChunk.hpp>
#include <Nazara/TerrainRenderer/TerrainRenderer.hpp>
#include <vector>
#include <queue>
#include <memory>

//FIX ME : Utiliser un arbre en structure interne pour le fast culling
//
class NAZARA_API NzTerrainChunksManager
{
    public:
        NzTerrainChunksManager(float edgelenght, unsigned int depth);
        ~NzTerrainChunksManager() = default;

        //virtual void AddToRenderQueue(NzRenderQueue& renderQueue) const;
        NzTerrainChunk* LocateChunk(NzVector2f location);
        void DrawChunks() const;

		const NzBoundingVolumef& GetGlobalBoundingBox() const;
		//unsigned int GetFreeBuffersAmount() const;
		//virtual nzSceneNodeType GetSceneNodeType() const;

        //bool Initialize();

        //NzVertexBuffer* QueryFreeBuffer();

        //bool RemovePatch(const NzTerrainNodeID& ID);
        //void ReturnBuffer(NzVertexBuffer* buffer);

        //FIX ME : Renommer en AddMesh
        //bool SubmitPatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID);

        //FIX ME : Il n'est pas utile de réenvoyer toutes les vertices lors d'un update
        //Avec un index buffer 16 fois plus grand contenant les 16 configuration
        //il est possible en donnant un offset à la donnée d'utiliser un différent index de ses vertices
        //bool UpdatePatch(const std::array<float,150>& subBuffer, const NzTerrainNodeID& ID);

    protected:
        //virtual bool VisibilityTest(const NzFrustumf& frustum);

        //NzBoundingBoxf m_aabb;
    private:
        //bool m_isReady;
        unsigned int m_depth;
        float m_edgeLenght;

        //std::vector<std::unique_ptr<NzTerrainNode>> m_zones;
        std::vector<NzTerrainChunk> m_chunks;

        NzBoundingVolumef m_aabb;

        //unsigned int m_bufferSize;
        //unsigned int m_patchSize;
        //unsigned int m_patchAmount;
};

#endif // NAZARA_TERRAINCHUNKSMANAGER_HPP
