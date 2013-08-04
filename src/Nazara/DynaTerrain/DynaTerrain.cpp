// Copyright (C) 2013  BEGES Rémi
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/DynaTerrain/DynaTerrain.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/Debug.hpp>

namespace
{
    unsigned int m_terrainMinimalPrecision = 2;
    //unsigned int maxSlopePrecision;

    unsigned int m_cameraMaximalPrecision = 6;
    unsigned int m_cameraRadiusAmount = 3;

    float m_cameraSmallerRadius = 50.f;
    float m_cameraRadiusSizeIncrement = 2.f;
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

unsigned int NzDynaTerrain::s_moduleReferenceCounter = 0;
