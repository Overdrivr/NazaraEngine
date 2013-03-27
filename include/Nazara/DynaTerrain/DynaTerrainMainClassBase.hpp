// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNATERRAINMAINCLASSBASE_HPP
#define NAZARA_DYNATERRAINMAINCLASSBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/3D/SceneNode.hpp>
#include <Nazara/2D/Drawable.hpp>

/// BASE CLASS OF THE TERRAIN/PLANET

//TODO : modifier shader du terrain pour qu'il fonctionne avec n'importe quelle direction (slope, altitude)
//FIX ME : Nommage des méthodes innaproprié, renforcer la sécurité
//FIX ME : sortir les shaders d'ici

class NAZARA_API NzDynaTerrainMainClassBase : public NzDrawable, public NzSceneNode
{
    public:
        NzDynaTerrainMainClassBase();
        ~NzDynaTerrainMainClassBase();

        virtual void AddToRenderQueue(NzRenderQueue& renderQueue) const;

        virtual const NzBoundingBoxf& GetBoundingBox() const;
		virtual nzSceneNodeType GetSceneNodeType() const;

		virtual void Draw() const;

        virtual void Initialize(const NzDynaTerrainConfigurationBase& configuration);

        bool SetShaders(const NzString& vertexShader, const NzString& fragmentShader);

        virtual void Update(const NzVector3f& cameraPosition);

    protected:

        void CreateIndexBuffer(unsigned int bufferCapacity, bool appendConfigurations = false);

        virtual bool VisibilityTest(const NzFrustumf& frustum);

        NzBoundingBoxf m_aabb;
    private:

        NzTexture m_terrainTexture;

        NzIndexBuffer* m_indexBuffer;
        unsigned int m_bufferCapacity;

        NzShader m_shader;
};

#endif // NAZARA_DYNATERRAINMAINCLASSBASE_HPP
