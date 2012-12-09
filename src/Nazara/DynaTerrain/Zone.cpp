// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#include "Zone.hpp"

NzZone::NzZone()
{
    //ctor
}

NzZone::~NzZone()
{
    //dtor
}

void NzZone::AddBuffer(NzVertexBuffer* buffer)
{

}

void NzZone::AddPatch(const std::array<float,150>& vertices, const id& ID)
{

}

void NzZone::DrawBuffers()
{

}

unsigned int NzZone::GetFreeBuffersAmount()
{

}

bool NzZone::RemoveFreeBuffer(NzVertexBuffer* buffer)
{

}

bool NzZone::RemovePatch(const id& ID)
{

}

bool NzZone::UpdatePatch(const std::array<float,150>& vertices, const id& ID)
{

}
