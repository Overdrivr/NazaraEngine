// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "Node.hpp"
#include "QuadTree.hpp"
#include <stack>
#include <iostream>
//#include <Nazara/DynaTerrain/Node.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

using namespace std;

int NzNode::nbNodes = 0;

NzNode::NzNode(NzQuadTree* quad, NzNode* parent, NzHeightSource* heightSource, const NzVector2f& center, const NzVector2f& size, nzLocation loc) : antiInfiniteLoop(200)
{
    m_location = loc;
    m_center = center;
    m_size = size;
    m_isLeaf = false;
    m_patchMemoryAllocated = false;
    m_doNotRefine = false;

    m_associatedQuadTree = quad;

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
    m_heightSource = heightSource;
    this->CreatePatch(center,size);
    //On lui affecte la source de bruit
    m_patch->SetHeightSource(heightSource);
    //On le fait calculer ses hauteurs et sa variation moyenne de pente
    m_patch->ComputeSlope();
}

NzNode::~NzNode()
{
    //cout<<"Deleting Node : "<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<endl;
    nbNodes--;
    m_associatedQuadTree->UnRegisterNode(this);
    if(m_isLeaf)
        m_associatedQuadTree->UnRegisterLeaf(this);
    if(m_patchMemoryAllocated)
        delete m_patch;

}


void NzNode::CleanTree(unsigned int minDepth)
{
    if(m_nodeID.lvl >= minDepth)
    {
        if(!m_isLeaf)
        {
            if(m_topLeftLeaf->IsLeaf())
            {
                m_associatedQuadTree->UnRegisterLeaf(m_topLeftLeaf);
                delete m_topLeftLeaf;
            }
            else
            {
                m_topLeftLeaf->CleanTree(minDepth);
                delete m_topLeftLeaf;
            }

            if(m_topRightLeaf->IsLeaf())
            {
                m_associatedQuadTree->UnRegisterLeaf(m_topRightLeaf);
                delete m_topRightLeaf;
            }
            else
            {
                m_topRightLeaf->CleanTree(minDepth);
                delete m_topRightLeaf;
            }

            if(m_bottomLeftLeaf->IsLeaf())
            {
                m_associatedQuadTree->UnRegisterLeaf(m_bottomLeftLeaf);
                delete m_bottomLeftLeaf;
            }
            else
            {
                m_bottomLeftLeaf->CleanTree(minDepth);
                delete m_bottomLeftLeaf;
            }

            if(m_bottomRightLeaf->IsLeaf())
            {
                m_associatedQuadTree->UnRegisterLeaf(m_bottomRightLeaf);
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
        m_associatedQuadTree->RegisterLeaf(this);
    }
}

void NzNode::CreatePatch(const NzVector2f& center, const NzVector2f& size)
{
    if(!m_patchMemoryAllocated)
    {
        m_patchMemoryAllocated = true;
        m_patch = new NzPatch(center,size);
    }
}

void NzNode::DeletePatch()
{
    if(m_patchMemoryAllocated)
    {
        m_patchMemoryAllocated = false;
        delete m_patch;
    }
}

void NzNode::Display()
{
    if(m_isLeaf)
    {
        if(m_patchMemoryAllocated)
        {
            //m_patch->Display();
        }
        else
        {
            cout<<"EXCEPTION : Trying to display non-allocated patch"<<endl;
        }
    }
    else
    {
        cout<<"EXCEPTION : Trying to display non-leaf node, error in quadtree leaves list"<<endl;
    }
}

NzNode* NzNode::GetChild(nzLocation location)
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

unsigned int NzNode::GetLevel() const
{
    return m_nodeID.lvl;
}

int NzNode::GetNodeAmount()
{
    return nbNodes;
}

const id& NzNode::GetNodeID() const
{
    return m_nodeID;
}

bool NzNode::TestNodeIDIsOutsideQuadTree(id nodeId)
{
    return nodeId.lvl < 0 || nodeId.sx < 0 || nodeId.sy < 0 || nodeId.sx > (std::pow(2,nodeId.lvl)-1) || nodeId.sy > (std::pow(2,nodeId.lvl)-1);
}

NzNode* NzNode::GetParent()
{
    return m_parent;
}

void NzNode::HierarchicalSubdivide(unsigned int maxDepth,bool eraseMemory)
{
    if(m_isLeaf == true && m_nodeID.lvl < maxDepth)
    {
        this->Subdivide(eraseMemory);
        m_topLeftLeaf->HierarchicalSubdivide(maxDepth,eraseMemory);
        m_topRightLeaf->HierarchicalSubdivide(maxDepth,eraseMemory);
        m_bottomLeftLeaf->HierarchicalSubdivide(maxDepth,eraseMemory);
        m_bottomRightLeaf->HierarchicalSubdivide(maxDepth,eraseMemory);
    }
}

bool NzNode::IsLeaf() const
{
    return m_isLeaf;
}

bool NzNode::IsRoot() const
{
    return m_isRoot;
}

void NzNode::SlopeBasedHierarchicalSubdivide(unsigned int maxDepth)
{
    //Si le node est une feuille
    if(m_isLeaf == true)
    {
        //Si son niveau est inférieur au niveau max de subdivision
            //Et également inférieur au niveau minimum de précision requis par la pente du terrain
                //Alors on le subdivise
        if(m_nodeID.lvl < maxDepth && m_nodeID.lvl < m_patch->GetTerrainConstrainedMinDepth())
        {
            m_doNotRefine = true;
            this->Subdivide(true);
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

bool NzNode::Subdivide(bool eraseMemory)
{
    if(m_isLeaf)
    {
        m_isLeaf = false;
        m_associatedQuadTree->UnRegisterLeaf(this);

        //if(eraseMemory)
        this->DeletePatch();

        if(m_topLeftLeaf == nullptr)
        {
            //On crée le premier node fils
            m_topLeftLeaf = new NzNode(m_associatedQuadTree,this,m_heightSource,
                                       NzVector2f(m_center.x-m_size.x/2.f,m_center.y+m_size.y/2.f),
                                       m_size/2.f,TOPLEFT);
            //C'est une subdivision, le node est forcément une leaf
            m_topLeftLeaf->m_isLeaf = true;

            //Et on l'enregistre auprès du quadtree
            m_associatedQuadTree->RegisterLeaf(m_topLeftLeaf);
            cout<<"creating topleft "<<m_nodeID.lvl+1<<endl;

            //On vérifie que le voisin de gauche est suffisamment subdivisé/refiné pour qu'il y ait au max 1 niveau d'écart entre les 2
            m_topLeftLeaf->HandleNeighborSubdivision(LEFT);
            //Traitement du voisin TOP
            m_topLeftLeaf->HandleNeighborSubdivision(TOP);
        }
        else
        {
            cout<<"Node::Subdivide topleft problem"<<endl;
        }

        if(m_topRightLeaf == nullptr)
        {
            m_topRightLeaf = new NzNode(m_associatedQuadTree,this,m_heightSource,
                                        NzVector2f(m_center.x+m_size.x/2.f,m_center.y+m_size.y/2.f),
                                        m_size/2.f,TOPRIGHT);
            m_topRightLeaf->m_isLeaf = true;
            m_associatedQuadTree->RegisterLeaf(m_topRightLeaf);
            cout<<"creating topright "<<m_nodeID.lvl+1<<endl;
            m_topRightLeaf->HandleNeighborSubdivision(RIGHT);
            m_topRightLeaf->HandleNeighborSubdivision(TOP);

        }
        else
        {
            cout<<"Node::Subdivide topright problem"<<endl;
        }

        if(m_bottomLeftLeaf == nullptr)
        {
            m_bottomLeftLeaf = new NzNode(m_associatedQuadTree,this,m_heightSource,
                                          NzVector2f(m_center.x-m_size.x/2.f,m_center.y-m_size.y/2.f),
                                          m_size/2.f,BOTTOMLEFT);
            m_bottomLeftLeaf->m_isLeaf = true;
            m_associatedQuadTree->RegisterLeaf(m_bottomLeftLeaf);
            cout<<"creating bottomleft "<<m_nodeID.lvl+1<<endl;
            m_bottomLeftLeaf->HandleNeighborSubdivision(LEFT);
            m_bottomLeftLeaf->HandleNeighborSubdivision(BOTTOM);
        }
        else
        {
            cout<<"Node::Subdivide bottomleft problem"<<endl;
        }

        if(m_bottomRightLeaf == nullptr)
        {
            m_bottomRightLeaf = new NzNode(m_associatedQuadTree,this,m_heightSource,
                                           NzVector2f(m_center.x+m_size.x/2.f,m_center.y-m_size.y/2.f),
                                           m_size/2.f,BOTTOMRIGHT);
            m_bottomRightLeaf->m_isLeaf = true;
            m_associatedQuadTree->RegisterLeaf(m_bottomRightLeaf);
            cout<<"creating bottomright "<<m_nodeID.lvl+1<<endl;
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

void NzNode::Refine(bool eraseMemory)
{
    if(!m_isLeaf && !m_doNotRefine)
    {
        m_isLeaf = true;
        m_associatedQuadTree->RegisterLeaf(this);

        //if(eraseMemory)
        //{
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


       /* }
        else
        {
            m_topLeftLeaf->m_isLeaf = false;
            m_topRightLeaf->m_isLeaf = false;
            m_bottomLeftLeaf->m_isLeaf = false;
            m_bottomRightLeaf->m_isLeaf = false;

            m_associatedQuadTree->UnRegisterLeaf(m_topLeftLeaf);
            m_associatedQuadTree->UnRegisterLeaf(m_topRightLeaf);
            m_associatedQuadTree->UnRegisterLeaf(m_bottomLeftLeaf);
            m_associatedQuadTree->UnRegisterLeaf(m_bottomRightLeaf);

            m_associatedQuadTree->UnRegisterNode(m_topLeftLeaf);
            m_associatedQuadTree->UnRegisterNode(m_topRightLeaf);
            m_associatedQuadTree->UnRegisterNode(m_bottomLeftLeaf);
            m_associatedQuadTree->UnRegisterNode(m_bottomRightLeaf);
        }*/
    }
}

void NzNode::HandleNeighborSubdivision(nzDirection direction)
{
    id tempID;
    NzNode* tempNode;
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

        tempNode = m_associatedQuadTree->GetNode(tempID);
        //Si le voisin n'existe pas
        if(tempNode == nullptr)
        {

            //cout<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<" un niveau detecte avec "<<tempID.lvl<<"|"<<tempID.sx<<"|"<<tempID.sy<<endl;
            //Un niveau d'écart n'est pas suffisant pour demander une subdivision
            tempID.lvl -= 1;
            tempID.sx /= 2;
            tempID.sy /= 2;
            tempNode = m_associatedQuadTree->GetNode(tempID);
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
                    tempNode = m_associatedQuadTree->GetNode(tempID);
                }

                if(counter < antiInfiniteLoop)
                {
                    cout<<"Resubdivision de "<<tempID.lvl<<"|"<<tempID.sx<<"|"<<tempID.sy<<" de "<<counter<<" niveau(x)"<<endl;
                    //On subdivise le node jusqu'à atteindre le bon niveau
                    tempNode->HierarchicalSubdivide(m_nodeID.lvl-1);
                }
                else
                {
                    cout<<"EXCEPTION : NzNode::HandleNeighborSubdivision ENTREE EN BOUCLE INFINIE"<<endl;
                    return;
                }
            }
            //else le node voisin est suffisamment divisé
        }
    }
    else
    {
        //DANS LE CAS OU PLUSIEURS QUADTREE SONT RATTACHES, IL FAUT RAJOUTER DU CODE ICI
    }
}

