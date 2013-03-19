// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNAMICTERRAIN_HPP
#define NAZARA_DYNAMICTERRAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/DynaTerrain/HeightSource2D.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/DynaTerrain/TerrainConfiguration.hpp>
#include <Nazara/DynaTerrain/DynaTerrainQuadTreeBase.hpp>
#include <Nazara/DynaTerrain/DynaTerrainMainClassBase.hpp>
#include <Nazara/DynaTerrain/Dispatcher.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/Renderer/Texture.hpp>

class NAZARA_API NzDynamicTerrain : public NzDynaTerrainMainClassBase
{
    public:

        NzDynamicTerrain(const NzTerrainConfiguration& configuration, NzHeightSource2D* heightSource);
        ~NzDynamicTerrain();

        void Initialize();

        void Render();

        void Update(const NzVector3f& cameraPosition);

    private:


        NzTerrainConfiguration m_configuration;
        NzHeightSource2D* m_heightSource;
        NzTexture m_terrainTexture;

        NzShader m_shader;
        NzDynaTerrainQuadTreeBase* quadtree;
        NzDynaTerrainQuadTreeBase* quadtree2;
};

#endif // NAZARA_DYNAMICTERRAIN_HPP
