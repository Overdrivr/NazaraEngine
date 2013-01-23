// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPHERE_HPP
#define NAZARA_SPHERE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzSphere
{
	public:
		NzSphere() = default;
		NzSphere(T X, T Y, T Z, T Radius);
		NzSphere(const T sphere[4]);
		NzSphere(const NzVector3<T>& center, T Radius);
		template<typename U> explicit NzSphere(const NzSphere<U>& sphere);
		NzSphere(const NzSphere& sphere) = default;
		~NzSphere() = default;

		bool Contains(T X, T Y, T Z) const;
		bool Contains(const NzVector3<T>& point) const;
		bool Contains(const NzCube<T>& cube) const;
		bool Contains(const NzSphere& sphere() const;

		NzSphere& ExtendTo(const NzVector3<T>& point);

        NzCube<T> GetBoundingCube() const;
		NzVector3<T> GetCenter() const;
		T GetRadius() const;
		T GetSquaredRadius() const;

		bool Intersect(const NzCube<T>& cube) const;
		bool Intersect(const NzSphere& sphere) const;

		bool IsValid() const;

		NzSphere& MakeZero();

		NzSphere& Set(T X, T Y, T Z, T Radius);
		NzSphere& Set(const T sphere[4]);
		NzSphere& Set(const NzVector3<T>& center, T Radius);
		template<typename U> NzSphere& Set(const NzSphere<U>& sphere);

		NzString ToString() const;

		operator NzString() const;

		//T& operator[](unsigned int i);//FIX ME : NO SEGFAULT ?
		//T operator[](unsigned int i) const;

		NzSphere operator*(T scalar) const;

		NzSphere& operator*=(T scalar);

		bool operator==(const NzSphere& sphere) const;
		bool operator!=(const NzSphere& sphere) const;

		static NzSphere Lerp(const NzSphere& from, const NzSphere& to, T interpolation);
		static NzSphere Zero();

		T x, y, z, radius;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzSphere<T>& sphere);

typedef NzSphere<double> NzSphered;
typedef NzSphere<float> NzSpheref;
typedef NzSphere<int> NzSpherei;
typedef NzSphere<unsigned int> NzCubeui;

#include <Nazara/Math/Sphere.inl>

#endif // NAZARA_SPHERE_HPP

