// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/NoiseTools.hpp>
#include <Nazara/Noise/Debug.hpp>

int fastfloor(float n)
{
    return (n >= 0) ? static_cast<int>(n) : static_cast<int>(n-1);
}

int JenkinsHash(int a, int b, int c)
{
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 13);
    b = b-c;  b = b - a;  b = b^(a << 8);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 13);
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 12);
    b = b-c;  b = b - a;  b = b^(a << 16);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 5);
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 3);
    b = b-c;  b = b - a;  b = b^(a << 10);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 15);
    return c;
}
