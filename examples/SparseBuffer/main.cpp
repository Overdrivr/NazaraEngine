#include <iostream>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Core/String.hpp>
//#include <Nazara/Utility/SparseBuffer.hpp>
#include <Nazara/DynaTerrain/SparseBuffer.hpp>
#include <array>

using namespace std;

int main()
{
    ///Example d'application du SparseBuffer
    //Notre pool de mémoire, et son image
    std::array<NzString,20> buffer;
    NzSparseBuffer<unsigned int> buffer_map(20);

    ///Ajout dans le pool du mot "neuf" utilisant la clé 9
    int freeIndex = buffer_map.InsertValueKey(9);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "neuf";

    ///Ajout dans le pool du mot "trois" utilisant la clé 3
    freeIndex = buffer_map.InsertValueKey(3);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "trois";

    ///Ajout dans le pool du mot "six" utilisant la clé 6
    freeIndex = buffer_map.InsertValueKey(6);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "six";

    cout<<"----- Affichage 1 ------"<<endl;
    //On affiche le contenu par lots d'objets (ici NzString) consécutifs
    //Si des objets sont dans le même lot, ils seront affichés sur la même ligne
    std::list<NzVector2ui>::const_iterator it = buffer_map.GetFilledBatches().cbegin();
    int lot = 0;
    for(it = buffer_map.GetFilledBatches().cbegin() ; it != buffer_map.GetFilledBatches().cend() ; ++it)
    {
        //On affiche un lot de NzString
        //(*it).x représente l'index de départ
        //(*it).y le nombre de NzString contenues dans le lot
        cout<<"Lot "<<lot<<" : ";
        for(int i((*it).x) ; i < (*it).x + (*it).y ; ++i)
        {
            cout<<"["<<buffer.at(i)<<"] ";
        }
        cout<<endl;
        lot++;
    }

    ///Ajout dans le pool du mot "Nazara" utilisant la clé 1
    freeIndex = buffer_map.InsertValueKey(1);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "Nazara";

    //Suppression dans le pool de la valeur de la clé 3
    int filledIndex = buffer_map.RemoveValueKey(3);
    if(filledIndex > 0)
    {
        //Cette étape est inutile si on affiche par lots, car cette valeur ne sera alors jamais affichée
        buffer.at(filledIndex) = "removed";
    }

    cout<<"------ Affichage 2 ------"<<endl;
    it = buffer_map.GetFilledBatches().cbegin();
    lot = 0;
    for(it = buffer_map.GetFilledBatches().cbegin() ; it != buffer_map.GetFilledBatches().cend() ; ++it)
    {
        //Contrairement au premier affichage, ici la suppression de la valeur ayant pour clé 3 a provoqué la création d'un nouveau lot
        //on a donc deux lots à afficher au lieu d'un
        cout<<"Lot "<<lot<<" : ";
        for(int i((*it).x) ; i < (*it).x + (*it).y ; ++i)
        {
            cout<<"["<<buffer.at(i)<<"] ";
        }
        cout<<endl;
        lot++;
    }

    return 0;
}
