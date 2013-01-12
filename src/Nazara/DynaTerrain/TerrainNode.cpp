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
    m_realCenter.x = center.x;
    m_realCenter.y = center.y;
    m_realCenter.z = 0.f;
    m_size = size;
    m_isLeaf = false;
    m_patchMemoryAllocated = false;
    m_doNotRefine = false;

    for(int i(0) ; i < 4 ; ++i)
        m_children[i] = nullptr;

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

    m_realCenter = m_patch->GetRealCenter();
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
            for(int i(0) ; i < 4 ; ++i)
            {
                if(m_children[i]->IsLeaf())
                    m_data->quadtree->UnRegisterLeaf(m_children[i]);
                else
                    m_children[i]->CleanTree(minDepth);

                delete m_children[i];
                m_children[i] = nullptr;
            }
        }
    }
    else if(!m_isLeaf)
    {
        for(int i(0) ; i < 4 ; ++i)
            m_children[i]->CleanTree(minDepth);
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
    return m_children[location];
}

const NzVector2f& NzTerrainNode::GetCenter() const
{
    return m_center;
}

const NzVector2f& NzTerrainNode::GetSize() const
{
    return m_size;
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

const NzVector3f& NzTerrainNode::GetRealCenter() const
{
    return m_realCenter;
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
    if(m_isLeaf == true)
    {
        if(m_nodeID.lvl < maxDepth)
        {
            //m_doNotRefine = true;
            this->Subdivide();

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSubdivide(maxDepth);
        }
    }
    else
    {
        for(int i(0) ; i < 4 ; ++i)
            m_children[i]->HierarchicalSubdivide(maxDepth);
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

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->SlopeBasedHierarchicalSubdivide(maxDepth);
        }
    }
    else
    {
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->SlopeBasedHierarchicalSubdivide(maxDepth);
    }
}

bool NzTerrainNode::Subdivide()
{
    if(m_isLeaf)
    {
        m_isLeaf = false;
        m_data->quadtree->UnRegisterLeaf(this);

        this->DeletePatch();

        if(m_children[TOPLEFT] == nullptr)
        {
            //On crée le premier node fils
            m_children[TOPLEFT] = new NzTerrainNode(m_data,this,NzVector2f(m_center.x-m_size.x/4.f,m_center.y+m_size.y/4.f),m_size/2.f,TOPLEFT);
            //C'est une subdivision, le node est forcément une leaf
            m_children[TOPLEFT]->m_isLeaf = true;

            //Et on l'enregistre auprès du quadtree
            m_data->quadtree->RegisterLeaf(m_children[TOPLEFT]);
            //cout<<"creating topleft "<<m_nodeID.lvl+1<<endl;

            //On vérifie que le voisin de gauche est suffisamment subdivisé/refiné pour qu'il y ait au max 1 niveau d'écart entre les 2
            m_children[TOPLEFT]->HandleNeighborSubdivision(LEFT);
            //Traitement du voisin TOP
            m_children[TOPLEFT]->HandleNeighborSubdivision(TOP);
        }
        else
        {
            cout<<"QuadCell::Subdivide topleft problem"<<endl;
        }

        if(m_children[TOPRIGHT] == nullptr)
        {
            m_children[TOPRIGHT] = new NzTerrainNode(m_data,this,NzVector2f(m_center.x+m_size.x/4.f,m_center.y+m_size.y/4.f),m_size/2.f,TOPRIGHT);
            m_children[TOPRIGHT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[TOPRIGHT]);
            //cout<<"creating topright "<<m_nodeID.lvl+1<<endl;
            m_children[TOPRIGHT]->HandleNeighborSubdivision(RIGHT);
            m_children[TOPRIGHT]->HandleNeighborSubdivision(TOP);

        }
        else
        {
            cout<<"QuadCell::Subdivide topright problem"<<endl;
        }

        if(m_children[BOTTOMLEFT] == nullptr)
        {
            m_children[BOTTOMLEFT] = new NzTerrainNode(m_data,this,NzVector2f(m_center.x-m_size.x/4.f,m_center.y-m_size.y/4.f),m_size/2.f,BOTTOMLEFT);
            m_children[BOTTOMLEFT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[BOTTOMLEFT]);
            //cout<<"creating bottomleft "<<m_nodeID.lvl+1<<endl;
            m_children[BOTTOMLEFT]->HandleNeighborSubdivision(LEFT);
            m_children[BOTTOMLEFT]->HandleNeighborSubdivision(BOTTOM);
        }
        else
        {
            cout<<"QuadCell::Subdivide bottomleft problem"<<endl;
        }

        if(m_children[BOTTOMRIGHT] == nullptr)
        {
            m_children[BOTTOMRIGHT] = new NzTerrainNode(m_data,this,NzVector2f(m_center.x+m_size.x/4.f,m_center.y-m_size.y/4.f),m_size/2.f,BOTTOMRIGHT);
            m_children[BOTTOMRIGHT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[BOTTOMRIGHT]);
            //cout<<"creating bottomright "<<m_nodeID.lvl+1<<endl;
            m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(RIGHT);
            m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(BOTTOM);

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

        for(int i(0) ; i < 4 ; ++i)
        {
            m_children[i]->DeletePatch();
            delete m_children[i];
            m_children[i] = nullptr;
        }
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

void NzTerrainNode::HierarchicalAddToCameraList(const NzCirclef& cameraRadius, unsigned int indexRadius)
{
    NzRectf tester(m_center - m_size/2.f, m_center + m_size/2.f);

    if(cameraRadius.Contains(tester))
    {
        this->HierarchicalAddAllChildrenToCameraList(indexRadius);
    }
    else if(cameraRadius.Intersect(tester))
    {
        //Si c'est un node feuille, on l'ajoute à la liste
        if(m_isLeaf)
        {
            if(m_nodeID.lvl < indexRadius)
            {
                m_data->quadtree->AddLeaveToCameraList(this);
            }

        }
        else
        {
            if(m_nodeID.lvl + 1 > indexRadius)//Il est trop subdivisé, on l'ajoute à la liste de refine
                m_data->quadtree->AddLeaveToCameraList(this,false);
            else
            {
                //Sinon on teste ses enfants
                for(int i(0) ; i < 4 ; ++i)
                    m_children[i]->HierarchicalAddToCameraList(cameraRadius,indexRadius);
            }
        }
    }
    else if(tester.Contains(cameraRadius))
    {
        //Le rayon de la caméra est entièrement contenu dans le node
        //Ne se produit qu'en haut de l'arbre
         if(m_isLeaf)
         {
             //Si on est ici, ça veut dire que le node est largement trop peu précis
             if(m_nodeID.lvl < indexRadius)
                m_data->quadtree->AddLeaveToCameraList(this);
         }
        else
        {
            if(m_nodeID.lvl + 1 > indexRadius)//Il est trop subdivisé, on l'ajoute à la liste de refine
                m_data->quadtree->AddLeaveToCameraList(this,false);
            else
            {
                //Sinon on teste ses enfants
                for(int i(0) ; i < 4 ; ++i)
                    m_children[i]->HierarchicalAddToCameraList(cameraRadius,indexRadius);
            }
        }
    }
    //else la méthode s'arrête là pour le node
}

void NzTerrainNode::HierarchicalAddAllChildrenToCameraList(unsigned int indexRadius)
{
    //Si c'est un node feuille
    if(m_isLeaf)
    {
        //Si il n'est pas assez subdivisé, on l'ajoute à la liste
        if(m_nodeID.lvl < indexRadius)
        {
            m_data->quadtree->AddLeaveToCameraList(this);
        }
    }
    else
    {
        if(m_nodeID.lvl + 1 > indexRadius)//Il est trop subdivisé, on l'ajoute à la liste de refine
            m_data->quadtree->AddLeaveToCameraList(this,false);
        else
        {
            //Sinon on teste ses enfants
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalAddAllChildrenToCameraList(indexRadius);
        }
    }
}

