#include <iostream>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Core/String.hpp>
//#include <Nazara/Utility/SparseBufferSet.hpp>
#include <Nazara/DynaTerrain/SparseBufferSet.hpp>
#include <array>

using namespace std;

int main()
{
     ///Exemple d'application du SparseBufferSet

    //Notre pool de mémoire
    std::array<std::array<NzString,20>,2> buffers;
    //La map de notre pool
    NzSparseBufferSet<unsigned int> buffer_map;
    buffer_map.AddEmptyBuffer(20);
    buffer_map.AddEmptyBuffer(20);

    //Ajout dans le pool du mot "neuf" utilisant la clé 9
    NzVector2i freeLocation = buffer_map.InsertValueKey(9);
    if(freeLocation.x > -1)
        buffers.at(freeLocation.x).at(freeLocation.y) = "neuf";

    //Ajout dans le pool du mot "trois" utilisant la clé 3
    freeLocation = buffer_map.InsertValueKey(3);
    if(freeLocation.x > -1)
        buffers.at(freeLocation.x).at(freeLocation.y) = "trois";

    //Ajout dans le pool du mot "six" utilisant la clé 6
    freeLocation = buffer_map.InsertValueKey(6);
    if(freeLocation.x > -1)
        buffers.at(freeLocation.x).at(freeLocation.y) = "six";

    cout<<"----- Affichage 1 ------"<<endl;
    //On affiche le contenu par lots
    for(unsigned int j(0) ; j < buffer_map.GetBufferAmount() ; ++j)
    {
        std::cout<<"Buffer "<<j<<std::endl;
        std::list<NzVector2ui>::const_iterator it = buffer_map.at(j).GetFilledBatches().cbegin();
        int lot = 0;
        for(it = buffer_map.at(j).GetFilledBatches().cbegin() ; it != buffer_map.at(j).GetFilledBatches().cend() ; ++it)
        {
            //On affiche un lot de NzString
            //(*it).x représente l'index de départ
            //(*it).y le nombre de NzString contenues dans le lot
            cout<<"Lot "<<lot<<" ";
            for(unsigned int i((*it).x) ; i < (*it).x + (*it).y ; ++i)
            {
                cout<<"["<<buffers.at(j).at(i)<<"] ";
            }
            cout<<endl;
            lot++;
        }
    }

    return 0;
}
