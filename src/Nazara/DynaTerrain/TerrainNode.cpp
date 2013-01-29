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
    if(m_isInitialized)
        DeletePatch();
}

void NzTerrainNode::DebugDrawAABB(bool leafOnly, int level)
{
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

const NzVector2f& NzTerrainNode::GetCenter() const
{
    return m_center;
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

void NzTerrainNode::Initialize(TerrainNodeData *data, NzTerrainNode* parent, const NzVector2f& center, float size, nzLocation loc)
{
    m_data = data;
    m_location = loc;
    m_center = center;
    m_realCenter.x = center.x;
    m_realCenter.y = center.y;
    m_realCenter.z = 0.f;
    m_size = size;
    m_isLeaf = false;

    if(m_patchMemoryAllocated)
        DeletePatch();//FIX ME : Reutiliser la mémoire au lieu d'en réallouer un, économie d'un PatchPool

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

    m_isInitialized = true;
}

void NzTerrainNode::Invalidate()
{
    m_isInitialized = false;
}

void NzTerrainNode::SlopeBasedHierarchicalSubdivide(unsigned int maxDepth)
{
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
            //m_children[TOPLEFT] = new NzTerrainNode();
            m_children[TOPLEFT] = m_data->quadtree->GetNodeFromPool();
            m_children[TOPLEFT]->Initialize(m_data,this,NzVector2f(m_center.x-m_size/4.f,m_center.y+m_size/4.f),m_size/2.f,TOPLEFT);
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
            std::cout<<"QuadCell::Subdivide topleft problem"<<std::endl;
        }

        if(m_children[TOPRIGHT] == nullptr)
        {
            //m_children[TOPRIGHT] = new NzTerrainNode();
            m_children[TOPRIGHT] = m_data->quadtree->GetNodeFromPool();
            m_children[TOPRIGHT]->Initialize(m_data,this,NzVector2f(m_center.x+m_size/4.f,m_center.y+m_size/4.f),m_size/2.f,TOPRIGHT);
            m_children[TOPRIGHT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[TOPRIGHT]);
            //cout<<"creating topright "<<m_nodeID.lvl+1<<endl;
            m_children[TOPRIGHT]->HandleNeighborSubdivision(RIGHT);
            m_children[TOPRIGHT]->HandleNeighborSubdivision(TOP);

        }
        else
        {
            std::cout<<"QuadCell::Subdivide topright problem"<<std::endl;
        }

        if(m_children[BOTTOMLEFT] == nullptr)
        {
            //m_children[BOTTOMLEFT] = new NzTerrainNode();
            m_children[BOTTOMLEFT] = m_data->quadtree->GetNodeFromPool();
            m_children[BOTTOMLEFT]->Initialize(m_data,this,NzVector2f(m_center.x-m_size/4.f,m_center.y-m_size/4.f),m_size/2.f,BOTTOMLEFT);
            m_children[BOTTOMLEFT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[BOTTOMLEFT]);
            //cout<<"creating bottomleft "<<m_nodeID.lvl+1<<endl;
            m_children[BOTTOMLEFT]->HandleNeighborSubdivision(LEFT);
            m_children[BOTTOMLEFT]->HandleNeighborSubdivision(BOTTOM);
        }
        else
        {
            std::cout<<"QuadCell::Subdivide bottomleft problem"<<std::endl;
        }

        if(m_children[BOTTOMRIGHT] == nullptr)
        {
            //m_children[BOTTOMRIGHT] = new NzTerrainNode();
            m_children[BOTTOMRIGHT] = m_data->quadtree->GetNodeFromPool();
            m_children[BOTTOMRIGHT]->Initialize(m_data,this,NzVector2f(m_center.x+m_size/4.f,m_center.y-m_size/4.f),m_size/2.f,BOTTOMRIGHT);
            m_children[BOTTOMRIGHT]->m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(m_children[BOTTOMRIGHT]);
            //cout<<"creating bottomright "<<m_nodeID.lvl+1<<endl;
            m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(RIGHT);
            m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(BOTTOM);

        }
        else
        {
            std::cout<<"Node::Subdivide bottomright problem"<<std::endl;
        }
        return true;
    }
    return false;
}

void NzTerrainNode::Refine()
{
    if(!m_isLeaf && !m_doNotRefine)
    {
        if(m_children[0]->m_isLeaf)
        {
            m_isLeaf = true;
            m_data->quadtree->RegisterLeaf(this);

            CreatePatch();
            m_patch->UploadMesh();

            for(int i(0) ; i < 4 ; ++i)
            {
                m_children[i]->DeletePatch();
                m_data->quadtree->ReturnNodeToPool(m_children[i]);
                //delete m_children[i];
                m_children[i] = nullptr;
            }
        }
        else
        {
            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->Refine();
        }

    }
    else if(!m_isLeaf && m_doNotRefine)
    {
        //Le node n'est pas refine-able à cause de la pente, mais ses enfants le sont peut être
        for(int i(0) ; i < 4 ; ++i)
                m_children[i]->Refine();
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
                    std::cout<<"EXCEPTION : NzTerrainNode::HandleNeighborSubdivision ENTREE EN BOUCLE INFINIE"<<std::endl;
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

void NzTerrainNode::HierarchicalAddToCameraList(const NzCubef& cameraFOV, unsigned int maximumDepth)
{
    //std::cout<<tester<<std::endl;
    //std::cout<<cameraFOV<<std::endl;

    if(cameraFOV.Contains(m_aabb))
    {
        //std::cout<<"1"<<std::endl;
        this->HierarchicalAddAllChildrenToCameraList(maximumDepth);
    }
    else if(cameraFOV.Intersect(m_aabb))
    {
        //std::cout<<"2"<<std::endl;
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
        //std::cout<<"3"<<std::endl;
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

