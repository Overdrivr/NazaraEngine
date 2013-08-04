// Copyright (C) 2013 BEGES Rémi
// This file is part of the "Nazara Engine - DynaTerrain module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNATERRAIN_HPP
#define NAZARA_DYNATERRAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>

//TODO : Ajouter la possibilité de changer la longeur d'un radius à la main à partir de son indice

class NAZARA_API NzDynaTerrain
{
	public:
		NzDynaTerrain() = delete;
		~NzDynaTerrain() = delete;

        static void ConfigurePrecisionSettings(unsigned int maximalPrecision,
                                               unsigned int radiusAmount,
                                               float smallerRadius,
                                               float radiusSizeIncrement);
		static bool Initialize();
		static bool IsInitialized();

		static void Uninitialize();


	private:
        static void ComputeRadii();

		static unsigned int s_moduleReferenceCounter;
};

#endif // NAZARA_DYNATERRAIN_HPP
