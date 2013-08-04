// Copyright (C) 2013  BEGES Rémi
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/DynaTerrain/DynaTerrain.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/DynaTerrain/ObjectPool.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

namespace
{
    unsigned int m_terrainMinimalPrecision = 2;
    //unsigned int maxSlopePrecision;
    unsigned int m_maximalPrecision = 6;
    unsigned int m_radiusAmount = 3;
    float m_smallerRadius = 50.f;
    float m_radiusSizeIncrement = 2.f;

    std::map<float,unsigned int> m_precisionRadii;

    NzObjectPool<NzTerrainInternalNode> m_nodesPool;
    NzObjectPool<NzPatch> m_patchesPool;
    NzObjectPool<NzTerrainVertex> m_verticesPool;
}

static void NzDynaTerrain::ConfigurePrecisionSettings(unsigned int maximalPrecision, unsigned int radiusAmount,
                                                      float smallerRadius, float radiusSizeIncrement)
{
    m_maximalPrecision = maximalPrecision;
    m_radiusAmount = radiusAmount;
    m_smallerRadius = smallerRadius;
    m_radiusSizeIncrement = radiusSizeIncrement;

    NzDynaTerrain::ComputeRadii();
}

static unsigned int NzDynaTerrain::GetPrecisionLevelFromDistance(float distance)
{
    if(distance > m_cameraRadiuses.rbegin()->first)
        return -1;

    if(distance < m_cameraRadiuses.begin()->first)
        return m_maximalPrecision;

    it = m_cameraRadiuses.lower_bound(distance);

    if(it != m_cameraRadiuses.end())
        return it->second;

    return -2;
}

bool NzDynaTerrain::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	if (!NzTerrainRenderer::Initialize())
	{
		NazaraError("Failed to initialize terrain renderer module");
		return false;
	}

	// Initialisation du module

	NazaraNotice("Initialized: DynaTerrain module");

	return true;
}

bool NzDynaTerrain::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzDynaTerrain::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	NazaraNotice("Uninitialized: DynaTerrain module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

static void NzDynaTerrain::ComputeRadii()
{
    float radius = m_smallerRadius;

    for(unsigned int i(0) ; i < m_radiusAmount ; ++i)
    {
        // Attention, on utilise la longueur du radius commé clé et non pas comme valeur
        // Ca permet de déterminer immédiatement à quel rayon appartient un point
        m_precisionRadii[radius] = m_maximalPrecision - i;

        radius *= m_radiusSizeIncrement;
    }
}

unsigned int NzDynaTerrain::s_moduleReferenceCounter = 0;
