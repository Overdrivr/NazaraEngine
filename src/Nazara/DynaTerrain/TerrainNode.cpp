// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainNode.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <stack>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>

int NzTerrainNode::nbNodes = 0;

NzTerrainNode::NzTerrainNode() : antiInfiniteLoop(200)
{
    nbNodes++;
    m_patchMemoryAllocated = false;
    m_isInitialized = false;
}

NzTerrainNode::~NzTerrainNode()
{
    nbNodes--;
    //if(m_isInitialized)
    //DeletePatch();
}

void NzTerrainNode::DebugDrawAABB(bool leafOnly, int level)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::DebugDrawAABB : Calling uninitialized node"<<std::endl;
        return;
    }

    if(leafOnly)
    {
        if(m_isLeaf)
        {
            NzDebugDrawer::Draw(m_aabb);
        }
        else
        {
            for(int i(0) ;  i < 4 ; ++i)
                m_children[i]->DebugDrawAABB(leafOnly,level);
        }
    }
    else
    {
        if(m_nodeID.lvl == level)
            NzDebugDrawer::Draw(m_aabb);
        else if(m_nodeID.lvl < level && m_isLeaf)
        {
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->DebugDrawAABB(leafOnly,level);
        }

    }
}


void NzTerrainNode::CleanTree(unsigned int minDepth)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::CleanTree : Calling uninitialized node"<<std::endl;
        return;
    }

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

                m_data->quadtree->ReturnNodeToPool(m_children[i]);
                //delete m_children[i];
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

void NzTerrainNode::CreatePatch()
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::CreatePatch : Calling uninitialized node"<<std::endl;
        return;
    }

    if(!m_patchMemoryAllocated)
    {
        m_patchMemoryAllocated = true;
        m_patch = m_data->quadtree->GetPatchFromPool();
        m_patch->Initialize(m_center,m_size,m_nodeID,m_data);
        m_aabb = m_patch->GetAABB();
    }
}

void NzTerrainNode::DeletePatch()
{
    if(m_patchMemoryAllocated)
    {
        m_patch->UnUploadMesh();
        m_patch->Invalidate();
        m_patchMemoryAllocated = false;
        m_data->quadtree->ReturnPatchToPool(m_patch);
    }
}

NzCubef NzTerrainNode::GetAABB() const
{
    return m_aabb;
}

NzTerrainNode* NzTerrainNode::GetChild(nzLocation location)
{
    return m_children[location];
}

NzTerrainNode* NzTerrainNode::GetChild(unsigned int i)
{
    if(i < 4)
        return m_children[i];
    else
        return nullptr;
}

const NzVector2f& NzTerrainNode::GetCenter() const
{
    return m_center;
}

NzTerrainNode* NzTerrainNode::GetNeighbor(nzDirection direction)
{
     if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::GetNeighbor : Calling uninitialized node"<<std::endl;
        return nullptr;
    }

    id tempID;
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
    }

    NzTerrainNode* neighbor;

    if(!m_data->quadtree->Contains(tempID))
    {

        neighbor = m_data->quadtree->GetNode(tempID);
        //Si le voisin n'existe pas (il n'y a pas de node voisin de même profondeur)
        if(neighbor == nullptr)
        {
            return nullptr;
        }
        else
        {
            return neighbor;
        }
    }
    else
    {
        return nullptr;
        //DANS LE CAS OU PLUSIEURS QUADTREE SONT RATTACHES, IL FAUT RAJOUTER DU CODE ICI
    }
}

float NzTerrainNode::GetSize() const
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

NzTerrainNode* NzTerrainNode::GetParent()
{
    return m_parent;
}

void NzTerrainNode::HierarchicalSubdivide(unsigned int maxDepth, bool registerAsDynamic)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalSubdivide : Calling uninitialized node"<<std::endl;
        return;
    }

    if(m_isLeaf)
    {
        if(m_nodeID.lvl < maxDepth)
        {
            //m_doNotRefine = true;
            this->Subdivide(registerAsDynamic);

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSubdivide(maxDepth,registerAsDynamic);
        }
    }
    else
    {
        for(int i(0) ; i < 4 ; ++i)
            m_children[i]->HierarchicalSubdivide(maxDepth,registerAsDynamic);
    }
}

bool NzTerrainNode::IsLeaf() const
{
    return m_isLeaf;
}

bool NzTerrainNode::IsMinimalPrecision() const
{
    return !m_doNotRefine;
}

bool NzTerrainNode::IsRoot() const
{
    return m_isRoot;
}

bool NzTerrainNode::IsRefineable() const
{
    return !m_doNotRefine && !m_isLeaf;
}

bool NzTerrainNode::IsValid() const
{
    return m_isInitialized;
}

void NzTerrainNode::Initialize(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, float size, nzLocation loc)
{
    m_isInitialized = true;
    m_data = data;
    m_location = loc;
    m_center = center;
    m_realCenter.x = center.x;//?
    m_realCenter.y = center.y;//?
    m_realCenter.z = 0.f;//?
    m_size = size;
    m_isLeaf = false;

    //DeletePatch();

    m_doNotRefine = false;

    for(int i(0) ; i < 4 ; ++i)
        m_children[i] = nullptr;

    if(parent == nullptr)
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

    //On crée son patch pour l'affichage
    CreatePatch();
    m_patch->UploadMesh();
}

void NzTerrainNode::Invalidate()
{
    DeletePatch();
    m_isInitialized = false;
}

void NzTerrainNode::HierarchicalSlopeBasedSubdivide(unsigned int maxDepth)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalSlopeBasedSubdivide : Calling uninitialized node"<<std::endl;
        return;
    }

    //Si la cellule est une feuille
    if(m_isLeaf == true)
    {
        //Si son niveau est inférieur au niveau max de subdivision
            //Et également inférieur au niveau minimum de précision requis par la pente du terrain
                //Alors on le subdivise
        if(m_nodeID.lvl < maxDepth && m_nodeID.lvl < static_cast<unsigned int>(m_patch->GetGlobalSlope() * m_data->quadtree->GetMaximumHeight()))
        {
            m_doNotRefine = true;
            this->Subdivide();

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSlopeBasedSubdivide(maxDepth);
        }
    }
    else
    {
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSlopeBasedSubdivide(maxDepth);
    }
}

bool NzTerrainNode::Subdivide(bool registerAsDynamic)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::Subdivide : Calling uninitialized node"<<std::endl;
        return false;
    }

    if(m_isLeaf)
    {
        m_isLeaf = false;
        m_data->quadtree->UnRegisterLeaf(this);
        this->DeletePatch();

        if(registerAsDynamic)
            m_data->quadtree->AddNodeToDynamicList(this);

        if(m_children[TOPLEFT] == nullptr)
        {
            //On crée le premier node fils
            //m_children[TOPLEFT] = new NzTerrainNode();
            m_children[TOPLEFT] = m_data->quadtree->GetNodeFromPool();
            m_children[TOPLEFT]->Initialize(m_data,this,NzVector2f(m_center.x-m_size/4.f,m_center.y-m_size/4.f),m_size/2.f,TOPLEFT);
            //C'est une subdivision, le node est forcément une leaf
            m_children[TOPLEFT]->m_isLeaf = true;

            //Et on l'enregistre auprès du quadtree
            m_data->quadtree->RegisterLeaf(m_children[TOPLEFT]);
            //cout<<"creating topleft "<<m_nodeID.lvl+1<<endl;

            //On vérifie que le voisin de gauche est suffisamment subdivisé/refiné pour qu'il y ait au max 1 niveau d'écart entre les 2
            m_children[TOPLEFT]->HandleNeighborSubdivision(LEFT,registerAsDynamic);
            //Traitement du voisin TOP
            m_children[TOPLEFT]->HandleNeighborSubdivision(TOP,registerAsDynamic);
        }
        else
        {
            std::cout<<"QuadCell::Subdivide topleft problem"<<std::endl;
        }

        if(m_children[TOPRIGHT] == nullptr)
        {
            //m_children[TOPRIGHT] = new NzTerrainNode();
            m_children[TOPRIGHT] = m_data->quadtree->GetNodeFromPool();
            m_children[TOPRIGHT]->Initialize(m_data,this,NzVector2f(m_center.x+m_size/4.f,m_center.y-m_size/4.f),m_size/2.f,TOPRIGHT);
            m_children[TOPRIGHT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[TOPRIGHT]);
            //cout<<"creating topright "<<m_nodeID.lvl+1<<endl;
            m_children[TOPRIGHT]->HandleNeighborSubdivision(RIGHT,registerAsDynamic);
            m_children[TOPRIGHT]->HandleNeighborSubdivision(TOP,registerAsDynamic);

        }
        else
        {
            std::cout<<"QuadCell::Subdivide topright problem"<<std::endl;
        }

        if(m_children[BOTTOMLEFT] == nullptr)
        {
            //m_children[BOTTOMLEFT] = new NzTerrainNode();
            m_children[BOTTOMLEFT] = m_data->quadtree->GetNodeFromPool();
            m_children[BOTTOMLEFT]->Initialize(m_data,this,NzVector2f(m_center.x-m_size/4.f,m_center.y+m_size/4.f),m_size/2.f,BOTTOMLEFT);
            m_children[BOTTOMLEFT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[BOTTOMLEFT]);
            //cout<<"creating bottomleft "<<m_nodeID.lvl+1<<endl;
            m_children[BOTTOMLEFT]->HandleNeighborSubdivision(LEFT,registerAsDynamic);
            m_children[BOTTOMLEFT]->HandleNeighborSubdivision(BOTTOM,registerAsDynamic);
        }
        else
        {
            std::cout<<"QuadCell::Subdivide bottomleft problem"<<std::endl;
        }

        if(m_children[BOTTOMRIGHT] == nullptr)
        {
            //m_children[BOTTOMRIGHT] = new NzTerrainNode();
            m_children[BOTTOMRIGHT] = m_data->quadtree->GetNodeFromPool();
            m_children[BOTTOMRIGHT]->Initialize(m_data,this,NzVector2f(m_center.x+m_size/4.f,m_center.y+m_size/4.f),m_size/2.f,BOTTOMRIGHT);
            m_children[BOTTOMRIGHT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[BOTTOMRIGHT]);
            //cout<<"creating bottomright "<<m_nodeID.lvl+1<<endl;
            m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(RIGHT,registerAsDynamic);
            m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(BOTTOM,registerAsDynamic);

        }
        else
        {
            std::cout<<"Node::Subdivide bottomright problem"<<std::endl;
        }
        return true;
    }
    return false;
}

bool NzTerrainNode::Refine()
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalRefine : Calling uninitialized node"<<std::endl;
        return false;
    }

    //Impossible de refiner une feuille
    if(m_isLeaf)
        return false;

    //Impossible de refiner autre chose qu'un parent d'une feuille
    for(int i(0) ; i < 4 ; ++i)
    {
        if(!m_children[i]->m_isLeaf)
            return false;

        if(m_children[i]->m_doNotRefine)
            return false;
    }

    nzDirection first, second;
    NzTerrainNode* temp = nullptr;

    //Impossible de refiner si les voisins ne sont pas d'accord
    for(int i(0) ; i < 4 ; ++i)
    {
        switch(m_children[i]->m_location)
        {
            case TOPLEFT:
                first = TOP;
                second = LEFT;
            break;

            case TOPRIGHT:
                first = TOP;
                second = RIGHT;
            break;

            case BOTTOMLEFT:
                first = BOTTOM;
                second = LEFT;
            break;

            case BOTTOMRIGHT:
                first = BOTTOM;
                second = RIGHT;
            break;

        }
        temp = m_children[i]->GetNeighbor(first);
        //Si il y a un node voisin de niveau égal
        if(temp != nullptr)
        {
             //Si il des fils
             if(!temp->m_isLeaf)
                return false;//Abandon, le refine va causer une différence de profondeur > 2
        }

        temp = m_children[i]->GetNeighbor(second);
        //Si il y a un node voisin de niveau égal
        if(temp != nullptr)
        {
             //Si il des fils
             if(!temp->m_isLeaf)
                return false;//Abandon, le refine va causer une différence de profondeur > 2
        }
    }

    for(int i(0) ; i < 4 ; ++i)
    {
        m_children[i]->DeletePatch();
        m_data->quadtree->ReturnNodeToPool(m_children[i]);
        m_children[i] = nullptr;
    }
    m_isLeaf = true;
    m_data->quadtree->RegisterLeaf(this);
    CreatePatch();
    m_patch->UploadMesh();

    //std::cout<<"Refined : "<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<std::endl;

    return true;

}

bool NzTerrainNode::HierarchicalRefine()
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalRefine : Calling uninitialized node"<<std::endl;
        return false;
    }

    //Impossible de refiner une feuille
    if(m_isLeaf)
        return false;

    //On refine les enfants d'abord
    for(int i(0) ; i < 4 ; ++i)
    {
        if(!m_children[i]->m_isLeaf)
        {
            m_children[i]->HierarchicalRefine();
        }
    }

    return this->Refine();

}

void NzTerrainNode::HandleNeighborSubdivision(nzDirection direction, bool registerAsDynamic)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HandleNeighborSubdivision : Calling uninitialized node"<<std::endl;
        return;
    }

    id tempID;
    NzTerrainNode* tempNode;
    tempID = m_nodeID;
    int counter = 0;
    nzDirection invDirection = direction;

    //CORRECT ?
    switch(direction)
    {
        case TOP :
            tempID.sy -= 1;
            invDirection = BOTTOM;
        break;

        case RIGHT :
            tempID.sx += 1;
            invDirection = LEFT;
        break;

        case BOTTOM :
            tempID.sy += 1;
            invDirection = TOP;
        break;

        case LEFT :
            tempID.sx -= 1;
            invDirection = RIGHT;
        break;

        default:
        break;
    }

    //std::cout<<"direction : "<<direction<<std::endl;

    //Si on ne cherche pas à atteindre une case externe
    if(!m_data->quadtree->Contains(tempID))
    {

        tempNode = m_data->quadtree->GetNode(tempID);
        //Si le voisin n'existe pas (il n'y a pas de node voisin de même profondeur)
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
                    tempNode->HierarchicalSubdivide(m_nodeID.lvl-1,registerAsDynamic);
                    //La subdivision a généré une interface, le node le plus subdivisé (cad this) doit s'adapter
                    //std::cout<<"Add interface 1 w/ dir"<<direction<<std::endl;
                    m_patch->SetConfiguration(direction,1);
                }
                else
                {
                    std::cout<<"EXCEPTION : NzTerrainNode::HandleNeighborSubdivision ENTREE EN BOUCLE INFINIE"<<std::endl;
                    return;
                }
            }
            else
            {
                //std::cout<<"Add interface 2 w/ dir"<<direction<<std::endl;
                m_patch->SetConfiguration(direction,1);
            }
            //else la cellule voisine voisin est suffisamment divisé
        }
        else
        {
            //La subdivision a supprimé une interface de précision, on l'indique au voisin qu'il n'a plus besoin de s'adapter

                //std::cout<<m_nodeID.lvl<<"|"<<m_nodeID.sx<<"|"<<m_nodeID.sy<<" VS "<<tempNode->m_nodeID.lvl<<"|"<<tempNode->m_nodeID.sx<<"|"<<tempNode->m_nodeID.sy<<std::endl;
                //std::cout<<"ca va chier"<<tempNode->m_nodeID.lvl<<" vs "<<m_nodeID.lvl<<std::endl;

            //std::cout<<direction<<" : "<<m_center<<" | "<<tempNode->m_center<<std::endl;
            if(tempNode->m_isLeaf)
            {
                //std::cout<<"Deleted interface w/ dir"<<direction<<std::endl;
                tempNode->m_patch->SetConfiguration(invDirection,0);
            }
            else
            {
                //std::cout<<"Add interface 3!! w/ dir"<<direction<<std::endl;
                //m_patch->SetConfiguration(direction,0);
            }

        }
    }
    else
    {
        //DANS LE CAS OU PLUSIEURS QUADTREE SONT RATTACHES, IL FAUT RAJOUTER DU CODE ICI
    }
}

void NzTerrainNode::HierarchicalAddToCameraList(const NzCubef& cameraFOV, unsigned int maximumDepth)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalAddToCameraList : Calling uninitialized node"<<std::endl;
        return;
    }

    if(cameraFOV.Contains(m_aabb))
    {
        this->HierarchicalAddAllChildrenToCameraList(maximumDepth);
    }
    else if(cameraFOV.Intersect(m_aabb))
    {
        //Si c'est un node feuille, on l'ajoute à la liste
        if(m_isLeaf)
        {
            if(m_nodeID.lvl < maximumDepth)
            {
                m_data->quadtree->AddLeaveToSubdivisionList(this);
            }
        }
        else
        {
            if(!(m_nodeID.lvl + 1 > maximumDepth))
            {
                //Sinon on teste ses enfants
                for(int i(0) ; i < 4 ; ++i)
                    m_children[i]->HierarchicalAddToCameraList(cameraFOV,maximumDepth);
            }
        }
    }
    else if(m_aabb.Contains(cameraFOV))
    {
        //Le rayon de la caméra est entièrement contenu dans le node
        //Ne se produit qu'en haut de l'arbre
         if(m_isLeaf)
         {
             //Si on est ici, ça veut dire que le node est largement trop peu précis
             if(m_nodeID.lvl < maximumDepth)
                m_data->quadtree->AddLeaveToSubdivisionList(this);
         }
        else
        {
            if(!(m_nodeID.lvl + 1 > maximumDepth))
            {
                //Sinon on teste ses enfants
                for(int i(0) ; i < 4 ; ++i)
                    m_children[i]->HierarchicalAddToCameraList(cameraFOV,maximumDepth);
            }
        }
    }
    //else la méthode s'arrête là pour le node
}

void NzTerrainNode::HierarchicalAddAllChildrenToCameraList(unsigned int maximumDepth)
{
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalAddAll... : Calling uninitialized node"<<std::endl;
        return;
    }

    //Si c'est un node feuille
    if(m_isLeaf)
    {
        //Si il n'est pas assez subdivisé, on l'ajoute à la liste
        if(m_nodeID.lvl < maximumDepth)
        {
            m_data->quadtree->AddLeaveToSubdivisionList(this);
        }
    }
    else
    {
        if(!(m_nodeID.lvl + 1 > maximumDepth))//Il est trop subdivisé, on l'ajoute à la liste de refine
        {
            //Sinon on teste ses enfants
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalAddAllChildrenToCameraList(maximumDepth);
        }
    }
}

