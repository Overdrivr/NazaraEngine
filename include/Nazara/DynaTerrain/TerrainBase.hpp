// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINBASE_HPP
#define NAZARA_TERRAINBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/DynaTerrain/Configuration/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

/// BASE CLASS OF THE TERRAIN/PLANET

//TODO : modifier shader du terrain pour qu'il fonctionne avec n'importe quelle direction (slope, altitude)
//FIX ME : Nommage des méthodes innaproprié, renforcer la sécurité

class NAZARA_API NzTerrainBase : public NzDrawable, public NzSceneNode
{
    public:
        NzTerrainBase();
        ~NzTerrainBase() = default;

        virtual const NzBoundingVolumef& GetBoundingVolume() const;
		virtual nzSceneNodeType GetSceneNodeType() const;

		virtual void Draw() const = 0;

        virtual void Update(const NzVector3f& cameraPosition) = 0;

    protected:
        virtual void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const;
        virtual bool VisibilityTest(const NzCamera* camera);

        NzBoundingVolumef m_aabb;
    private:

        //TODO : A BOUGER
        //NzTexture m_terrainTexture;
        //NzTextureSampler m_sampler;
};

#endif // NAZARA_TERRAINBASE_HPP
