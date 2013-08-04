// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TERRAINBASE_HPP
#define NAZARA_TERRAINBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/DynaTerrain/Configuration/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

/// BASE CLASS OF THE TERRAIN/PLANET

//TODO : Créer une seule fois l'index buffer pour tous les terrains
//TODO : modifier shader du terrain pour qu'il fonctionne avec n'importe quelle direction (slope, altitude)
//FIX ME : Nommage des méthodes innaproprié, renforcer la sécurité

class NAZARA_API NzTerrainBase : public NzDrawable, public NzSceneNode
{
    public:
        NzTerrainBase();
        ~NzTerrainBase() = default;

        virtual const NzBoundingVolumef& GetBoundingBox() const;
		virtual nzSceneNodeType GetSceneNodeType() const;

		virtual void Draw() const = 0;

        //virtual void Initialize(const NzDynaTerrainConfigurationBase& configuration);

        virtual void Update(const NzVector3f& cameraPosition);

    protected:
        virtual void AddToRenderQueue(NzRenderQueue& renderQueue) const;

        NzBoundingVolumef m_aabb;
    private:

        //TODO : A BOUGER
        //NzTexture m_terrainTexture;
        //NzTextureSampler m_sampler;
};

#endif // NAZARA_TERRAINBASE_HPP
