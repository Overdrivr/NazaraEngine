// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "TerrainNode.hpp"
#include "TerrainQuadTree.hpp"
#include <stack>
#include <iostream>
//#include <Nazara/DynaTerrain/QuadCell.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

int NzTerrainNode::nbNodes = 0;

NzTerrainNode::NzTerrainNode(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, const NzVector2f& size, nzLocation loc) : antiInfiniteLoop(200)
{
    m_data = data;
    m_location = loc;
    m_center = center;
    m_size = size;
    m_isLeaf = false;
    m_patchMemoryAllocated = false;
    m_doNotRefine = false;

    m_topLeftLeaf = nullptr;
    m_topRightLeaf = nullptr;
    m_bottomLeftLeaf = nullptr;
    m_bottomRightLeaf = nullptr;

    if(parent == 0)
    {
        m_isRoot = true;
        m_isLeaf = true;
        m_nodeID.lvl = 0;
        m_nodeID.sx = 0;
        m_nodeID.sy = 0;
    }
    else
    {
        m_nodeID.lvl = parent->GetLevel()+1;
        m_parent = parent;
        m_isRoot = false;
        int offx = 0, offy = 0;
        switch(m_location)
        {
            case TOPRIGHT:
                offx = 1;
            break;

            case BOTTOMLEFT:
                offy = 1;
            break;

            case BOTTOMRIGHT:
                offx = 1;
                offy = 1;
            break;

            default:
            break;
        }
        m_nodeID.sx = parent->m_nodeID.sx * 2 + offx;
        m_nodeID.sy = parent->m_nodeID.sy * 2 + offy;
    }
    nbNodes++;

    //On crée son patch pour l'affichage
    this->CreatePatch(center,size);
    //On lui affecte la source de bruit
    m_patch->SetData(m_data);
    //On le fait calculer ses hauteurs et sa variation moyenne de pente
    m_patch->ComputeSlope();
    //On uploade le patch sur le dispatcher
    m_patch->UploadMesh();
}

NzTerrainNode::~NzTerrainNode()
{
    //cout<<"Deleting QuadCell : "<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<endl;
    nbNodes--;
    m_data->quadtree->UnRegisterNode(this);
    if(m_isLeaf)
        m_data->quadtree->UnRegisterLeaf(this);
    if(m_patchMemoryAllocated)
        delete m_patch;

}


void NzTerrainNode::CleanTree(unsigned int minDepth)
{
    if(m_nodeID.lvl >= minDepth)
    {
        if(!m_isLeaf)
        {
            if(m_topLeftLeaf->IsLeaf())
            {
                m_data->quadtree->UnRegisterLeaf(m_topLeftLeaf);
                delete m_topLeftLeaf;
            }
            else
            {
                m_topLeftLeaf->CleanTree(minDepth);
                delete m_topLeftLeaf;
            }

            if(m_topRightLeaf->IsLeaf())
            {
                m_data->quadtree->UnRegisterLeaf(m_topRightLeaf);
                delete m_topRightLeaf;
            }
            else
            {
                m_topRightLeaf->CleanTree(minDepth);
                delete m_topRightLeaf;
            }

            if(m_bottomLeftLeaf->IsLeaf())
            {
                m_data->quadtree->UnRegisterLeaf(m_bottomLeftLeaf);
                delete m_bottomLeftLeaf;
            }
            else
            {
                m_bottomLeftLeaf->CleanTree(minDepth);
                delete m_bottomLeftLeaf;
            }

            if(m_bottomRightLeaf->IsLeaf())
            {
                m_data->quadtree->UnRegisterLeaf(m_bottomRightLeaf);
                delete m_bottomRightLeaf;
            }
            else
            {
                m_bottomRightLeaf->CleanTree(minDepth);
                delete m_bottomRightLeaf;
            }
        }
    }
    else if(!m_isLeaf)
    {
        m_topLeftLeaf->CleanTree(minDepth);
        m_topRightLeaf->CleanTree(minDepth);
        m_bottomLeftLeaf->CleanTree(minDepth);
        m_bottomRightLeaf->CleanTree(minDepth);
    }

    if(m_nodeID.lvl == minDepth)
    {
        m_isLeaf = true;
        m_data->quadtree->RegisterLeaf(this);
    }
}

void NzTerrainNode::CreatePatch(const NzVector2f& center, const NzVector2f& size)
{
    if(!m_patchMemoryAllocated)
    {
        m_patchMemoryAllocated = true;
        m_patch = new NzPatch(center,size,m_nodeID);
    }
}

void NzTerrainNode::DeletePatch()
{
    if(m_patchMemoryAllocated)
    {
        m_patch->UnUploadMesh();
        m_patchMemoryAllocated = false;
        delete m_patch;
    }
}

NzTerrainNode* NzTerrainNode::GetChild(nzLocation location)
{
    if(!m_isLeaf)
    {
        switch(location)
        {
            case TOPLEFT :
                return m_topLeftLeaf;
            break;

            case TOPRIGHT :
                return m_topRightLeaf;
            break;

            case BOTTOMLEFT :
                return m_bottomLeftLeaf;
            break;

            default :
                return m_bottomRightLeaf;
            break;
        }
    }
    else
        return this;
}

unsigned int NzTerrainNode::GetLevel() const
{
    return m_nodeID.lvl;
}

int NzTerrainNode::GetNodeAmount()
{
    return nbNodes;
}

const id& NzTerrainNode::GetNodeID() const
{
    return m_nodeID;
}

bool NzTerrainNode::TestNodeIDIsOutsideQuadTree(id nodeId)
{
    return nodeId.lvl < 0 || nodeId.sx < 0 || nodeId.sy < 0 || nodeId.sx > (std::pow(2,nodeId.lvl)-1) || nodeId.sy > (std::pow(2,nodeId.lvl)-1);
}

NzTerrainNode* NzTerrainNode::GetParent()
{
    return m_parent;
}

void NzTerrainNode::HierarchicalSubdivide(unsigned int maxDepth)
{
    if(m_isLeaf == true && m_nodeID.lvl < maxDepth)
    {
        m_doNotRefine = true;
        this->Subdivide();
        m_topLeftLeaf->HierarchicalSubdivide(maxDepth);
        m_topRightLeaf->HierarchicalSubdivide(maxDepth);
        m_bottomLeftLeaf->HierarchicalSubdivide(maxDepth);
        m_bottomRightLeaf->HierarchicalSubdivide(maxDepth);
    }
}

bool NzTerrainNode::IsLeaf() const
{
    return m_isLeaf;
}

bool NzTerrainNode::IsRoot() const
{
    return m_isRoot;
}

void NzTerrainNode::SlopeBasedHierarchicalSubdivide(unsigned int maxDepth)
{
    //Si la cellule est une feuille
    if(m_isLeaf == true)
    {
        //Si son niveau est inférieur au niveau max de subdivision
            //Et également inférieur au niveau minimum de précision requis par la pente du terrain
                //Alors on le subdivise
        if(m_nodeID.lvl < maxDepth && m_nodeID.lvl < m_patch->GetTerrainConstrainedMinDepth())
        {
            m_doNotRefine = true;
            this->Subdivide();
            m_topLeftLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
            m_topRightLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
            m_bottomLeftLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
            m_bottomRightLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
        }
    }
    else
    {
            m_topLeftLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
            m_topRightLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
            m_bottomLeftLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
            m_bottomRightLeaf->SlopeBasedHierarchicalSubdivide(maxDepth);
    }
}

bool NzTerrainNode::Subdivide()
{
    if(m_isLeaf)
    {
        m_isLeaf = false;
        m_data->quadtree->UnRegisterLeaf(this);

        this->DeletePatch();

        if(m_topLeftLeaf == nullptr)
        {
            //On crée le premier node fils
            m_topLeftLeaf = new NzTerrainNode(m_data,this,NzVector2f(m_center.x-m_size.x/4.f,m_center.y+m_size.y/4.f),m_size/2.f,TOPLEFT);
            //C'est une subdivision, le node est forcément une leaf
            m_topLeftLeaf->m_isLeaf = true;

            //Et on l'enregistre auprès du quadtree
            m_data->quadtree->RegisterLeaf(m_topLeftLeaf);
            //cout<<"creating topleft "<<m_nodeID.lvl+1<<endl;

            //On vérifie que le voisin de gauche est suffisamment subdivisé/refiné pour qu'il y ait au max 1 niveau d'écart entre les 2
            m_topLeftLeaf->HandleNeighborSubdivision(LEFT);
            //Traitement du voisin TOP
            m_topLeftLeaf->HandleNeighborSubdivision(TOP);
        }
        else
        {
            cout<<"QuadCell::Subdivide topleft problem"<<endl;
        }

        if(m_topRightLeaf == nullptr)
        {
            m_topRightLeaf = new NzTerrainNode(m_data,this,NzVector2f(m_center.x+m_size.x/4.f,m_center.y+m_size.y/4.f),m_size/2.f,TOPRIGHT);
            m_topRightLeaf->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_topRightLeaf);
            //cout<<"creating topright "<<m_nodeID.lvl+1<<endl;
            m_topRightLeaf->HandleNeighborSubdivision(RIGHT);
            m_topRightLeaf->HandleNeighborSubdivision(TOP);

        }
        else
        {
            cout<<"QuadCell::Subdivide topright problem"<<endl;
        }

        if(m_bottomLeftLeaf == nullptr)
        {
            m_bottomLeftLeaf = new NzTerrainNode(m_data,this,NzVector2f(m_center.x-m_size.x/4.f,m_center.y-m_size.y/4.f),m_size/2.f,BOTTOMLEFT);
            m_bottomLeftLeaf->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_bottomLeftLeaf);
            //cout<<"creating bottomleft "<<m_nodeID.lvl+1<<endl;
            m_bottomLeftLeaf->HandleNeighborSubdivision(LEFT);
            m_bottomLeftLeaf->HandleNeighborSubdivision(BOTTOM);
        }
        else
        {
            cout<<"QuadCell::Subdivide bottomleft problem"<<endl;
        }

        if(m_bottomRightLeaf == nullptr)
        {
            m_bottomRightLeaf = new NzTerrainNode(m_data,this,NzVector2f(m_center.x+m_size.x/4.f,m_center.y-m_size.y/4.f),m_size/2.f,BOTTOMRIGHT);
            m_bottomRightLeaf->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_bottomRightLeaf);
            //cout<<"creating bottomright "<<m_nodeID.lvl+1<<endl;
            m_bottomRightLeaf->HandleNeighborSubdivision(RIGHT);
            m_bottomRightLeaf->HandleNeighborSubdivision(BOTTOM);

        }
        else
        {
            cout<<"Node::Subdivide bottomright problem"<<endl;
        }
        return true;
    }
    return false;
}

void NzTerrainNode::Refine()
{
    if(!m_isLeaf && !m_doNotRefine)
    {
        m_isLeaf = true;
        m_data->quadtree->RegisterLeaf(this);

        this->CreatePatch(m_center,m_size);

        m_topLeftLeaf->DeletePatch();
        m_topRightLeaf->DeletePatch();
        m_bottomLeftLeaf->DeletePatch();
        m_bottomRightLeaf->DeletePatch();

        delete m_topLeftLeaf;
        delete m_topRightLeaf;
        delete m_bottomLeftLeaf;
        delete m_bottomRightLeaf;

        m_topLeftLeaf = nullptr;
        m_topRightLeaf = nullptr;
        m_bottomLeftLeaf = nullptr;
        m_bottomRightLeaf = nullptr;
    }
}

void NzTerrainNode::HandleNeighborSubdivision(nzDirection direction)
{
    id tempID;
    NzTerrainNode* tempNode;
    tempID = m_nodeID;
    int counter = 0;

    switch(direction)
    {
        case TOP :
            tempID.sy -= 1;
        break;

        case RIGHT :
            tempID.sx += 1;
        break;

        case BOTTOM :
            tempID.sy += 1;
        break;

        case LEFT :
            tempID.sx -= 1;
        break;

        default:
        break;
    }

    //Si on ne cherche pas à atteindre une case externe
    if(!TestNodeIDIsOutsideQuadTree(tempID))
    {

        tempNode = m_data->quadtree->GetNode(tempID);
        //Si le voisin n'existe pas
        if(tempNode == nullptr)
        {

            //cout<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<" un niveau detecte avec "<<tempID.lvl<<"|"<<tempID.sx<<"|"<<tempID.sy<<endl;
            //Un niveau d'écart n'est pas suffisant pour demander une subdivision
            tempID.lvl -= 1;
            tempID.sx /= 2;
            tempID.sy /= 2;
            tempNode = m_data->quadtree->GetNode(tempID);
            //cout<<" deuxieme test "<<tempID.lvl<<"|"<<tempID.sx<<"|"<<tempID.sy<<endl;

            if(tempNode == nullptr)
            {
                //cout<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<" 2+ niveaux detectes"<<tempID.lvl<<"|"<<tempID.sx<<"|"<<tempID.sy<<endl;
                while(tempNode == nullptr && counter < antiInfiniteLoop)//A partir de deux niveaux d'écarts on doit subdiviser
                {
                    counter++;
                    tempID.lvl -= 1;
                    tempID.sx /= 2;
                    tempID.sy /= 2;
                    tempNode = m_data->quadtree->GetNode(tempID);
                }

                if(counter < antiInfiniteLoop)
                {
                    //cout<<"Resubdivision de "<<tempID.lvl<<"|"<<tempID.sx<<"|"<<tempID.sy<<" de "<<counter<<" niveau(x)"<<endl;
                    //On subdivise la cellule jusqu'à atteindre le bon niveau
                    tempNode->HierarchicalSubdivide(m_nodeID.lvl-1);
                }
                else
                {
                    cout<<"EXCEPTION : NzTerrainNode::HandleNeighborSubdivision ENTREE EN BOUCLE INFINIE"<<endl;
                    return;
                }
            }
            //else la cellule voisine voisin est suffisamment divisé
        }
    }
    else
    {
        //DANS LE CAS OU PLUSIEURS QUADTREE SONT RATTACHES, IL FAUT RAJOUTER DU CODE ICI
    }
}

