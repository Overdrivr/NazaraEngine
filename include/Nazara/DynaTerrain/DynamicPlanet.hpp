// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNAMICPLANET_HPP
#define NAZARA_DYNAMICPLANET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/PlanetQuadTree.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/Renderer/Texture.hpp>

//TODO : HeightSource 2D et 3D
//TODO : Configuration pour planète
//TODO : modifier shader du terrain pour qu'il fonctionne avec n'importe quelle direction (slope, altitude)

class NAZARA_API NzDynamicPlanet
{
    public:

        NzDynamicPlanet(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource);
        ~NzDynamicPlanet();

        void Initialize();

        void Render();

        bool SetShaders(const NzString& vertexShader, const NzString& fragmentShader);

        void Update(const NzVector3f& cameraPosition);

    private:

        NzTerrainConfiguration m_configuration;
        NzHeightSource* m_heightSource;
        NzTexture m_terrainTexture;

        NzShader m_shader;
        NzPlanetQuadTree* quadtree;
        NzPlanetQuadTree* quadtree2;
};

#endif // NAZARA_DYNAMICPLANET_HPP
