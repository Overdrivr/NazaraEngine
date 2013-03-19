// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNATERRAINMAINCLASSBASE_HPP
#define NAZARA_DYNATERRAINMAINCLASSBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/DynaTerrainQuadTreeBase.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Renderer/Texture.hpp>

/// BASE CLASS OF THE TERRAIN/PLANET

//TODO : modifier shader du terrain pour qu'il fonctionne avec n'importe quelle direction (slope, altitude)
//FIX ME : Nommage des méthodes innaproprié, renforcer la sécurité

class NAZARA_API NzDynaTerrainMainClassBase
{
    public:

        NzDynaTerrainMainClassBase();
        ~NzDynaTerrainMainClassBase();

        virtual void Initialize(const NzDynaTerrainConfigurationBase& configuration);

        virtual void Render();

        bool SetShaders(const NzString& vertexShader, const NzString& fragmentShader);

        virtual void Update(const NzVector3f& cameraPosition);

    protected:

        void CreateIndexBuffer(unsigned int bufferCapacity, bool appendConfigurations = false);
    private:

        NzTexture m_terrainTexture;

        NzIndexBuffer* m_indexBuffer;
        unsigned int m_bufferCapacity;

        NzShader m_shader;
};

#endif // NAZARA_DYNATERRAINMAINCLASSBASE_HPP
