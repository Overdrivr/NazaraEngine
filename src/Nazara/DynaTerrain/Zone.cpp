// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#include "Zone.hpp"
#include <iostream>

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
    std::cout<<"Adding patch "<<ID.lvl<<"|"<<ID.sx<<"|"<<ID.sy<<std::endl;

    //Si la zone n'a pas de buffers de libre, elle en demande un au dispatcher
        //Si le dispatcher ne peut en fournir aucun, l'opération est temporairement abandonnée et le patch est sauvegardé dans une file
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
