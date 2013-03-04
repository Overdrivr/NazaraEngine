// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNAMICTERRAIN_HPP
#define NAZARA_DYNAMICTERRAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/Renderer/Texture.hpp>

class NAZARA_API NzDynamicTerrain
{
    public:

        NzDynamicTerrain(const NzTerrainConfiguration& configuration, NzHeightSource* heightSource);
        ~NzDynamicTerrain();

        void Initialize();

        void Render();

        bool SetShaders(const NzString& vertexShader, const NzString& fragmentShader);

        void Update(const NzVector3f& cameraPosition);

    private:


        NzTerrainConfiguration m_configuration;
        NzHeightSource* m_heightSource;
        NzTexture m_terrainTexture;

        NzShader m_shader;
        NzTerrainQuadTree* quadtree;
        NzTerrainQuadTree* quadtree2;
};

#endif // NAZARA_DYNAMICTERRAIN_HPP
