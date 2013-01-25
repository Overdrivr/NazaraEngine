// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <algorithm>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzSphere<T>::NzSphere(T X, T Y, T Z, T Radius)
{
    Set(X,Y,Z,Radius);
}

template<typename T>
NzSphere<T>::NzSphere(const T sphere[4])
{
    Set(sphere);
}

template<typename T>
NzSphere<T>::NzSphere(const NzVector3<T>& center, T Radius)
{
    Set(center,Radius);
}

template<typename T>
template<typename U>
NzSphere<T>::NzSphere(const NzSphere<U>& sphere)
{
    Set(sphere);
}

template<typename T>
bool NzSphere<T>::Contains(T X, T Y, T Z) const
{
    return NzVector3<T>(X - x, Y - y, Z - z).GetSquaredLenght() < GetSquaredRadius();
}

template<typename T>
bool NzSphere<T>::Contains(const NzVector3<T>& point) const
{
    return point.SquaredDistance(NzVector3<T>(x,y,z)) < GetSquaredRadius();
}

template<typename T>
bool NzSphere<T>::Contains(const NzCube<T>& cube) const
{
    if(!Contains(cube.GetBoundingSphere()) && !Intersect(cube.GetBoundingSphere()))
        return false;

    if(Contains(cube.GetBoundingSphere()))
        return true;

    //Brute force : il doit être possible de faire mieux (minkowski ?)
    if(!Contains(cube.GetPosition()))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(cube.width, F(0.0),      F(0.0))))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(cube.width, cube.height, F(0.0))))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(F(0.0),     cube.height, F(0.0))))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(F(0.0),     F(0.0),      cube.depth)))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(cube.width, F(0.0),      cube.depth)))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(cube.width, cube.height, cube.depth)))
        return false;
    if(!Contains(cube.GetPosition() + NzVector3<T>(F(0.0),     cube.height, cube.depth)))
        return false;

    return true;

}

template<typename T>
bool NzSphere<T>::Contains(const NzSphere& sphere) const
{
    return sphere.GetCenter().Distance(GetCenter()) + sphere.GetRadius() <= GetRadius();
}

template<typename T>
NzSphere<T>& NzSphere<T>::ExtendTo(const NzVector3<T>& point)
{
    radius = std::max(point.Distance(GetCenter()), radius);
    return *this;
}

template<typename T>
NzCube<T> NzSphere<T>::GetBoundingCube() const
{
    return NzCube<T>(x - radius,
                     y - radius,
                     z - radius,
                     2 * radius,
                     2 * radius,
                     2 * radius);
}

template<typename T>
NzVector3<T> NzSphere<T>::GetCenter() const
{
    return NzVector3<T>(x,y,z);
}

template<typename T>
T NzSphere<T>::GetRadius() const
{
    return radius;
}

template<typename T>
T NzSphere<T>::GetSquaredRadius() const
{
    return radius * radius;
}

template<typename T>
bool NzSphere<T>::Intersect(const NzCube<T>& cube) const
{
    T dmin(0.0);
	T dmax(0.0);
	bool face = false;
	T sqrt_radius = std::sqrt(radius);
	T a,b;

	NzVector3<T> corner(cube.x + cube.width, cube.y + cube.height, cube.z + cube.depth);

	for(unsigned int i(0) ; i < 3 ; ++i)
	{
		a = std::sqrt(*(&x+i) - cube[i] );
		b = std::sqrt(*(&x+i) - corner[i] );

		dmax = std::max(a,b);

		if(*(&x+i) < cube[i])
		{
			face = true;
			dmin = a;
		}
		else if(*(&x+i) > corner[i])
		{
			face = true;
			dmin = b;
		}
		else if(std::min(a,b) <= sqrt_radius)
			face = true;
	}

	if(face && (dmin <= sqrt_radius) && (sqrt_radius <= dmax))
		return true;

	return false;
}

template<typename T>
bool NzSphere<T>::Intersect(const NzSphere& sphere) const
{
    return sphere.GetCenter().Distance(GetCenter()) - sphere.GetRadius() < GetRadius() &&
           sphere.GetCenter().Distance(GetCenter()) + sphere.GetRadius() > GetRadius();
}

template<typename T>
bool NzSphere<T>::IsValid() const
{
    return radius > F(0.0);
}

template<typename T>
NzSphere<T>& NzSphere<T>::MakeZero()
{
    x = F(0.0);
    y = F(0.0);
    z = F(0.0);
    radius = F(0.0);
    return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::Set(T X, T Y, T Z, T Radius)
{
    x = X;
    y = Y;
    z = Z;
    radius = Radius;
    return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::Set(const T sphere[4])
{
    x = sphere[0];
    y = sphere[1];
    z = sphere[2];
    radius = sphere[3];
    return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::Set(const NzVector3<T>& center, T Radius)
{
    x = center.x;
    y = center.y;
    z = center.z;
    radius = Radius;
    return *this;
}

template<typename T>
template<typename U>
NzSphere<T>& NzSphere<T>::Set(const NzSphere<U>& sphere)
{
    x = F(sphere.x);
    y = F(sphere.y);
    z = F(sphere.z);
    radius = F(sphere.radius);
    return *this;
}

template<typename T>
NzString NzSphere<T>::ToString() const
{
    NzStringStream ss;

	return ss << "Sphere(" << x << ", " << y << ", " << z << " | " << radius << ')';
}

template<typename T>
NzSphere<T>::operator NzString() const
{
    return ToString();
}


template<typename T>
T& NzSphere<T>::operator[](unsigned int i)
{
    #if NAZARA_MATH_SAFE
	if (i >= 4)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 4)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
T NzSphere<T>::operator[](unsigned int i) const
{
    #if NAZARA_MATH_SAFE
	if (i >= 4)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 4)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
NzSphere<T> NzSphere<T>::operator*(T scalar) const
{
    return NzSphere<T>(x,y,z,radius * scalar);
}

template<typename T>
NzSphere<T>& NzSphere<T>::operator*=(T scalar)
{
    radius *= scalar;
    return *this;
}

template<typename T>
bool NzSphere<T>::operator==(const NzSphere& sphere) const
{
    return NzNumberEquals(x, sphere.x) && NzNumberEquals(y, sphere.y) && NzNumberEquals(z, sphere.z) && NzNumberEquals(radius, sphere.radius);
}

template<typename T>
bool NzSphere<T>::operator!=(const NzSphere& cube) const
{
    return !operator==(cube);
}

template<typename T>
NzSphere<T> NzSphere<T>::Lerp(const NzSphere& from, const NzSphere& to, T interpolation)
{
    #ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzSphere sphere;
	sphere.x = NzLerp(from.x, to.x, interpolation);
	sphere.y = NzLerp(from.y, to.y, interpolation);
	sphere.z = NzLerp(from.z, to.z, interpolation);
	sphere.radius = NzLerp(from.radius, to.radius, interpolation);

	return sphere;
}

template<typename T>
NzSphere<T> NzSphere<T>::Zero()
{
    NzSphere sphere;
    return sphere.MakeZero();
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzSphere<T>& sphere)
{
    return out << sphere.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
