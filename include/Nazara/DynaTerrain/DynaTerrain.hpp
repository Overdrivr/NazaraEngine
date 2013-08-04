// Copyright (C) 2013 BEGES Rémi
// This file is part of the "Nazara Engine - DynaTerrain module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNATERRAIN_HPP
#define NAZARA_DYNATERRAIN_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>

class NAZARA_API NzDynaTerrain
{
	public:
		NzDynaTerrain() = delete;
		~NzDynaTerrain() = delete;

		static bool Initialize();

		static bool IsInitialized();

		static void Uninitialize();

	private:
		static unsigned int s_moduleReferenceCounter;
};

#endif // NAZARA_DYNATERRAIN_HPP
