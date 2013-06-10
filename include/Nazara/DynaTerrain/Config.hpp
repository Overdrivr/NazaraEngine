/*
	Nazara Engine - DynaTerrain

	Copyright (C) 2012 RÃ©mi "Overdrivr" BÃ¨ges (remi.beges@laposte.net)

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#ifndef NAZARA_CONFIG_DYNATERRAIN_HPP
#define NAZARA_CONFIG_DYNATERRAIN_HPP

/// Chaque modification d'un paramÃ¨tre du module nÃ©cessite une recompilation de celui-ci

// Utilise un tracker pour repÃ©rer les Ã©ventuels leaks (Ralentit l'exÃ©cution)
#define NAZARA_DYNATERRAIN_MEMORYLEAKTRACKER 1

// Active les tests de sÃ©curitÃ© basÃ©s sur le code (ConseillÃ© pour le dÃ©veloppement)
#define NAZARA_DYNATERRAIN_SAFE 1

#endif // NAZARA_CONFIG_MODULENAME_HPP
