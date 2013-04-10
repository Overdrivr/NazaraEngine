// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/DynaTerrain/Config.hpp>
#include <Nazara/DynaTerrain/TerrainInternalNode.hpp>
#include <Nazara/DynaTerrain/TerrainQuadTree.hpp>
#include <stack>
#include <iostream>
#include <Nazara/DynaTerrain/Debug.hpp>


int NzTerrainInternalNode::nbNodes = 0;

NzTerrainInternalNode::NzTerrainInternalNode()
{
    nbNodes++;
    m_isInitialized = false;
}

NzTerrainInternalNode::~NzTerrainInternalNode()
{
    nbNodes--;
}

void NzTerrainInternalNode::CleanTree(unsigned int minDepth)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::CleanTree : Calling uninitialized node"<<std::endl;
        return;
    }
    #endif

    if(m_nodeID.depth >= minDepth)
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
                m_children[i] = nullptr;
            }
        }
    }
    else if(!m_isLeaf)
    {
        for(int i(0) ; i < 4 ; ++i)
            m_children[i]->CleanTree(minDepth);
    }

    if(m_nodeID.depth == minDepth)
    {
        m_isLeaf = true;
        m_data->quadtree->RegisterLeaf(this);
    }
}

void NzTerrainInternalNode::CreatePatch()
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::CreatePatch : Calling uninitialized node"<<std::endl;
        return;
    }
    #endif

    m_patch = m_data->quadtree->GetPatchFromPool();
    m_patch->Initialize(m_nodeID,m_data);
    m_aabb = m_patch->GetAABB();
    m_patch->UploadMesh();
}

void NzTerrainInternalNode::DeletePatch()
{
    m_patch->UnUploadMesh();
    m_patch->Invalidate();
    m_data->quadtree->ReturnPatchToPool(m_patch);
}

const NzCubef& NzTerrainInternalNode::GetAABB() const
{
    return m_aabb;
}

NzTerrainInternalNode* NzTerrainInternalNode::GetChild(nzLocation location)
{
    return m_children[location];
}

NzTerrainInternalNode* NzTerrainInternalNode::GetChild(unsigned int i)
{
    if(i < 4)
        return m_children[i];
    else
        return nullptr;
}

NzTerrainInternalNode* NzTerrainInternalNode::GetDirectNeighbor(nzDirection direction)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::GetNeighbor : Calling uninitialized node"<<std::endl;
        return nullptr;
    }
    #endif

    NzTerrainNodeID tempID = m_nodeID;
    int counter = 0;

    switch(direction)
    {
        case TOP :
            tempID.locy -= 1;
        break;

        case RIGHT :
            tempID.locx += 1;
        break;

        case BOTTOM :
            tempID.locy += 1;
        break;

        case LEFT :
            tempID.locx -= 1;
        break;
    }

    NzTerrainInternalNode* neighbor;

    if(tempID.IsValid())
    {
        return m_data->quadtree->GetNode(tempID);
    }
    else
    {
        NzTerrainQuadTree* tempQuad = m_data->quadtree->GetContainingQuadTree(tempID);

        if(tempQuad == nullptr)
            return nullptr;

        //On convertit les coordonnées du node dans celles du quadtree voisin
        tempID.Normalize();
        return tempQuad->GetNode(tempID);
    }
}

int NzTerrainInternalNode::GetNodeAmount()
{
    return nbNodes;
}

const NzTerrainNodeID& NzTerrainInternalNode::GetNodeID() const
{
    return m_nodeID;
}

NzTerrainInternalNode* NzTerrainInternalNode::GetParent()
{
    return m_parent;
}

void NzTerrainInternalNode::HierarchicalSubdivide(unsigned int maxDepth, bool isNotReversible)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalSubdivide : Calling uninitialized node"<<std::endl;
        return;
    }
    #endif

    if(m_isLeaf)
    {
        if(m_nodeID.depth < maxDepth)
        {
            this->Subdivide(isNotReversible);

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->m_doNotRefine = isNotReversible;

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->HierarchicalSubdivide(maxDepth,isNotReversible);
        }
    }
    else
    {
        for(int i(0) ; i < 4 ; ++i)
            m_children[i]->HierarchicalSubdivide(maxDepth,isNotReversible);
    }
}

bool NzTerrainInternalNode::IsLeaf() const
{
    return m_isLeaf;
}

bool NzTerrainInternalNode::IsRoot() const
{
    return m_isRoot;
}

bool NzTerrainInternalNode::IsValid() const
{
    return m_isInitialized;
}

void NzTerrainInternalNode::Initialize(TerrainNodeData *data, NzTerrainInternalNode* parent, nzLocation loc)
{
    InitializeData(data,parent,loc);
    CreatePatch();
}

void NzTerrainInternalNode::Invalidate()
{
    DeletePatch();
    m_isInitialized = false;
}

void NzTerrainInternalNode::HierarchicalSlopeBasedSubdivide(unsigned int maxDepth)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalSlopeBasedSubdivide : Calling uninitialized node"<<std::endl;
        return;
    }
    #endif

    //Si la cellule est une feuille
    if(m_isLeaf == true)
    {
        //Si son niveau est inférieur au niveau max de subdivision
            //Et également inférieur au niveau minimum de précision requis par la pente du terrain
                //Alors on le subdivise
        if(m_nodeID.depth < maxDepth && m_nodeID.depth < static_cast<unsigned int>(m_patch->GetGlobalSlope() * m_data->quadtree->GetMaximumHeight()))
        {
            m_doNotRefine = true;//FIX ME : Affecter cette valeur aux enfants plutot ?
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

bool NzTerrainInternalNode::Subdivide(bool isNotReversible)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::Subdivide : Calling uninitialized node."<<std::endl;
        return false;
    }

    if(m_children[TOPLEFT] != nullptr ||
       m_children[TOPRIGHT] != nullptr ||
       m_children[BOTTOMLEFT] != nullptr ||
       m_children[BOTTOMRIGHT] != nullptr)
    {
       std::cout<<"NzTerrainNode::Subdivide : Trying to overwrite existing children."<<std::endl;
       return false;
    }
    #endif

    if(!m_isLeaf)
        return false;

    m_isLeaf = false;
    m_data->quadtree->UnRegisterLeaf(this);
    //this->DeletePatch();
    m_patch->UnUploadMesh();

    //On récupère des pointeurs valides pour les nodes
    m_children[TOPLEFT] = m_data->quadtree->GetNodeFromPool();
    m_children[TOPRIGHT] = m_data->quadtree->GetNodeFromPool();
    m_children[BOTTOMLEFT] = m_data->quadtree->GetNodeFromPool();
    m_children[BOTTOMRIGHT] = m_data->quadtree->GetNodeFromPool();

    m_children[TOPLEFT]->Initialize(m_data,this,*m_patch,TOPLEFT);
    m_children[TOPRIGHT]->Initialize(m_data,this,*m_patch,TOPRIGHT);
    m_children[BOTTOMLEFT]->Initialize(m_data,this,*m_patch,BOTTOMLEFT);
    m_children[BOTTOMRIGHT]->Initialize(m_data,this,*m_patch,BOTTOMRIGHT);

    //C'est une subdivision, le node est forcément une leaf
    m_children[TOPLEFT]->m_isLeaf = true;
    m_children[TOPRIGHT]->m_isLeaf = true;
    m_children[BOTTOMLEFT]->m_isLeaf = true;
    m_children[BOTTOMRIGHT]->m_isLeaf = true;

    //Et on l'enregistre auprès du quadtree
    m_data->quadtree->RegisterLeaf(m_children[TOPLEFT]);
    m_data->quadtree->RegisterLeaf(m_children[TOPRIGHT]);
    m_data->quadtree->RegisterLeaf(m_children[BOTTOMLEFT]);
    m_data->quadtree->RegisterLeaf(m_children[BOTTOMRIGHT]);

    //On vérifie que les nodes voisins n'aient pas plus d'1 niveau d'écart de profondeur
    m_children[TOPLEFT]->HandleNeighborSubdivision(LEFT,isNotReversible);
    m_children[TOPLEFT]->HandleNeighborSubdivision(TOP,isNotReversible);

    m_children[TOPRIGHT]->HandleNeighborSubdivision(RIGHT,isNotReversible);
    m_children[TOPRIGHT]->HandleNeighborSubdivision(TOP,isNotReversible);

    m_children[BOTTOMLEFT]->HandleNeighborSubdivision(LEFT,isNotReversible);
    m_children[BOTTOMLEFT]->HandleNeighborSubdivision(BOTTOM,isNotReversible);

    m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(RIGHT,isNotReversible);
    m_children[BOTTOMRIGHT]->HandleNeighborSubdivision(BOTTOM,isNotReversible);

    return true;
}

bool NzTerrainInternalNode::Refine()
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalRefine : Calling uninitialized node"<<std::endl;
        return false;
    }
    #endif

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
    NzTerrainInternalNode* temp = nullptr;

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
        temp = m_children[i]->GetDirectNeighbor(first);
        //Si il y a un node voisin de niveau égal
        if(temp != nullptr)
        {
             //Si il des fils
             if(!temp->m_isLeaf)
                return false;//Abandon, le refine va causer une différence de profondeur > 2
        }

        temp = m_children[i]->GetDirectNeighbor(second);
        //Si il y a un node voisin de niveau égal
        if(temp != nullptr)
        {
             //Si il des fils
             if(!temp->m_isLeaf)
                return false;//Abandon, le refine va causer une différence de profondeur > 2
        }
    }

    //On supprime les fils
    for(int i(0) ; i < 4 ; ++i)
    {
        m_data->quadtree->UnRegisterLeaf(m_children[i]);
        m_children[i]->Invalidate();
        m_data->quadtree->ReturnNodeToPool(m_children[i]);
        m_children[i] = nullptr;
    }

    //Ce node devient leaf
    m_isLeaf = true;
    m_data->quadtree->RegisterLeaf(this);
    //CreatePatch();
    m_patch->UploadMesh();

    //On met à jour les interfaces
    nzDirection dirDirection[4] = {TOP,BOTTOM,LEFT,RIGHT};
    nzDirection invDirection[4] = {BOTTOM,TOP,RIGHT,LEFT};

    //Contains location of neighbor's children in contact with (*this)
    nzLocation locLUT[4][2] =  {{BOTTOMLEFT,BOTTOMRIGHT},//TOP Neighbour
                                {TOPLEFT   ,TOPRIGHT},//BOTTOM Neighbour
                                {TOPRIGHT  ,BOTTOMRIGHT},//LEFT Neighbour
                                {TOPLEFT   ,BOTTOMLEFT}};//RIGHT Neighbour

    for(int i(0) ; i < 4 ; ++i)
    {
        //On signale aux voisins le refinement
        temp = this->GetDirectNeighbor(dirDirection[i]);

        if(temp != nullptr)
        {
            if(temp->m_isLeaf)
            {
                //This et son voisin auront le même niveau, on supprime l'interface ?
                //temp->m_patch->SetConfiguration(invDirection[i],0);
            }
            else
            {
                //This aura un niveau inférieur, on indique aux fils du voisin de générer une interface :
                if(temp->m_children[locLUT[i][0]]->m_isLeaf)
                {
                    temp->m_children[locLUT[i][0]]->m_patch->SetConfiguration(invDirection[i],1);
                }

                if(temp->m_children[locLUT[i][1]]->m_isLeaf)
                {
                    temp->m_children[locLUT[i][1]]->m_patch->SetConfiguration(invDirection[i],1);
                }
            }

        }
    }

    return true;

}

bool NzTerrainInternalNode::HierarchicalRefine()
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HierarchicalRefine : Calling uninitialized node."<<std::endl;
        return false;
    }
    #endif

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

void NzTerrainInternalNode::HandleNeighborSubdivision(nzDirection direction, bool isNotReversible)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::HandleNeighborSubdivision : Calling uninitialized node"<<std::endl;
        return;
    }
    #endif

    NzTerrainNodeID tempID = m_nodeID;
    int counter = 0;
    nzDirection invDirection = direction;

    switch(direction)
    {
        case TOP :
            tempID.locy -= 1;
            invDirection = BOTTOM;
        break;

        case RIGHT :
            tempID.locx += 1;
            invDirection = LEFT;
        break;

        case BOTTOM :
            tempID.locy += 1;
            invDirection = TOP;
        break;

        case LEFT :
            tempID.locx -= 1;
            invDirection = RIGHT;
        break;

        default:
        break;
    }

    NzTerrainQuadTree* tempQuad;
    NzTerrainInternalNode* tempNode;

    //Si on ne cherche pas à atteindre une case externe
    if(tempID.IsValid())
    {
        tempQuad = m_data->quadtree;
        tempNode = m_data->quadtree->GetNode(tempID);
    }
    else
    {
        tempQuad = m_data->quadtree->GetContainingQuadTree(tempID);

        if(tempQuad == nullptr)
            return;

        //On convertit les coordonnées du node dans celles du quadtree voisin
        tempID.Normalize();
        tempNode = tempQuad->GetNode(tempID);
    }


    //Si le voisin n'existe pas (il n'y a pas de node voisin de même profondeur)
    if(tempNode == nullptr)
    {
        //Un niveau d'écart n'est pas suffisant pour demander une subdivision
        tempID.depth -= 1;
        tempID.locx /= 2;
        tempID.locy /= 2;
        tempNode = tempQuad->GetNode(tempID);

        if(tempNode == nullptr)
        {
            while(tempNode == nullptr && counter < 200)
            {
                counter++;
                tempID.depth -= 1;
                tempID.locx /= 2;
                tempID.locy /= 2;
                tempNode = tempQuad->GetNode(tempID);
            }

            if(counter < 200)
            {
                //On subdivise la cellule jusqu'à atteindre le bon niveau
                tempNode->HierarchicalSubdivide(m_nodeID.depth-1,isNotReversible);
                //La subdivision a généré une interface, le node le plus subdivisé (cad this) doit s'adapter
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
            m_patch->SetConfiguration(direction,1);
        }
        //else la cellule voisine voisin est suffisamment divisé
    }
    else
    {
        //La subdivision a supprimé une interface de précision, on l'indique au voisin qu'il n'a plus besoin de s'adapter
        if(tempNode->m_isLeaf)
        {
            tempNode->m_patch->SetConfiguration(invDirection,0);
        }
    }
}

void NzTerrainInternalNode::Update(const NzVector3f& cameraPosition)
{
    #if NAZARA_DYNATERRAIN_SAFE
    if(!m_isInitialized)
    {
        std::cout<<"NzTerrainNode::Update : Calling uninitialized node."<<std::endl;
        return;
    }
    #endif

    //A) On calcule la précision optimale du node tenant compte de sa distance à la caméra
    float distance = m_aabb.DistanceTo(cameraPosition);

    int rayon = m_data->quadtree->TransformDistanceToCameraInRadiusIndex(distance);

    //B) Si la précision optimale est inférieure à la précision actuelle
        //Si le node est une feuille, on l'ajoute à la liste de subdivision
        //Sinon on update ses enfants
            //Le fusion pouvant échouer, on garantit que le node subdivisé ne reste pas dans la file de fusion
    if(m_nodeID.depth < rayon)
    {
        if(m_isLeaf)
        {
            m_data->quadtree->AddLeaveToSubdivisionQueue(this);
        }
        else
        {
            m_data->quadtree->TryRemoveNodeFromRefinementQueue(this);

            for(int i(0) ; i < 4 ; ++i)
                m_children[i]->Update(cameraPosition);
        }
    }
    //C) Si la précision optimale est supérieure ou égale à la précision actuelle
        //Si le node n'est pas une feuille, on l'ajoute à la liste de fusion
    else if(m_nodeID.depth >= rayon)
    {
        if(!m_isLeaf)
        {
            m_data->quadtree->AddNodeToRefinementQueue(this);
        }
    }
}

void NzTerrainInternalNode::Initialize(TerrainNodeData *data, NzTerrainInternalNode* parent, const NzPatch& patch, nzLocation loc)
{
    InitializeData(data,parent,loc);
    m_patch = m_data->quadtree->GetPatchFromPool();
    m_patch->InitializeFromParent(m_nodeID,m_data,patch);
    m_aabb = m_patch->GetAABB();
    m_patch->UploadMesh();
}

void NzTerrainInternalNode::InitializeData(TerrainNodeData *data, NzTerrainInternalNode* parent, nzLocation loc)
{
    m_isInitialized = true;
    m_data = data;
    m_location = loc;
    m_isLeaf = false;

    m_doNotRefine = false;

    for(int i(0) ; i < 4 ; ++i)
        m_children[i] = nullptr;

    if(parent == nullptr)
    {
        m_isRoot = true;
        m_isLeaf = true;
        m_nodeID.depth = 0;
        m_nodeID.locx = 0;
        m_nodeID.locy = 0;
    }
    else
    {
        m_nodeID.depth = parent->m_nodeID.depth + 1;
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
        }
        m_nodeID.locx = parent->m_nodeID.locx * 2 + offx;
        m_nodeID.locy = parent->m_nodeID.locy * 2 + offy;
    }
}

