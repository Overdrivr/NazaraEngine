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

    //Notre pool de NzString
    std::array<NzString,6> buffer;
    //La map de notre pool
    NzSparseBuffer<unsigned int> buffer_map(6);


    //Ajout dans le pool du mot "un" utilisant la clé 1
    int freeIndex = buffer_map.InsertValueKey(1);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "un";

        //Ajout dans le pool du mot "deux" utilisant la clé 2
    freeIndex = buffer_map.InsertValueKey(2);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "deux";

    //Ajout dans le pool du mot "trois" utilisant la clé 3
    freeIndex = buffer_map.InsertValueKey(3);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "trois";

             //Ajout dans le pool du mot "trois" utilisant la clé 3
    freeIndex = buffer_map.InsertValueKey(4);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "quatre";


    //Ajout dans le pool du mot "six" utilisant la clé 6
    freeIndex = buffer_map.InsertValueKey(5);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "cinq";


    //Ajout dans le pool du mot "six" utilisant la clé 6
    freeIndex = buffer_map.InsertValueKey(6);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "six";


    ///Choose any you want to remove and observe the way the buffer batches together the consecutives strings
    buffer_map.RemoveValueKey(1);
    buffer_map.RemoveValueKey(6);
    //buffer_map.RemoveValueKey(5);
    //buffer_map.RemoveValueKey(4);
    buffer_map.RemoveValueKey(2);
    buffer_map.RemoveValueKey(3);

    std::list<NzBatch>::const_iterator it = buffer_map.GetFilledBatches().cbegin();
    int lot = 0;
    cout<<"-------------- Display 1 -------------"<<endl;
    std::cout<<"FILLED BATCHES-------------"<<std::endl;

    for(it = buffer_map.GetFilledBatches().cbegin() ; it != buffer_map.GetFilledBatches().cend() ; ++it)
    {
        //Displaying NzString by batch
        //(*it).Start() is the starting index of the batch
        //(*it).Count() is the number of objects in the batch
        cout<<"Lot "<<lot<<" starts at "<<(*it).Start()<<" with "<<(*it).Count()<<" string(s) :";
        for(unsigned int i((*it).Start()) ; i < (*it).Start() + (*it).Count() ; ++i)
        {
            cout<<"["<<buffer.at(i)<<"] ";
        }
        cout<<endl;
        lot++;
    }

    it = buffer_map.GetFreeBatches().cbegin();
    lot = 0;

    std::cout<<"FREE BATCHES-----------------------"<<std::endl;

    for(it = buffer_map.GetFreeBatches().cbegin() ; it != buffer_map.GetFreeBatches().cend() ; ++it)
    {
        cout<<"Batch "<<lot<<" starts at "<<(*it).Start()<<" with "<<(*it).Count()<<" string(s)"<<endl;
        lot++;
    }
    cout<<"--------------------------------"<<endl;



    freeIndex = buffer_map.InsertValueKey(1);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "Nazara";
    else
        cout<<"not enough space !"<<endl;


   /int filledIndex = buffer_map.RemoveValueKey(3);

    freeIndex = buffer_map.InsertValueKey(12);
    if(freeIndex > -1)
        buffer.at(freeIndex) = "Is Awesome";
    else
        cout<<"not enough space !"<<endl;


  //int filledIndex = buffer_map.RemoveValueKey(3);


    std::list<NzBatch>::const_iterator it = buffer_map.GetFilledBatches().cbegin();
    int lot = 0;

    cout<<"-------------- Display 2 -------------"<<endl;
    std::cout<<"FILLED BATCHES-------------"<<std::endl;

    for(it = buffer_map.GetFilledBatches().cbegin() ; it != buffer_map.GetFilledBatches().cend() ; ++it)
    {
        cout<<"Lot "<<lot<<" starts at "<<(*it).Start()<<" with "<<(*it).Count()<<" string(s) :";
        for(unsigned int i((*it).Start()) ; i < (*it).Start() + (*it).Count() ; ++i)
        {
            cout<<"["<<buffer.at(i)<<"] ";
        }
        cout<<endl;
        lot++;
    }

    it = buffer_map.GetFreeBatches().cbegin();
    std::cout<<"FREE BATCHES-----------------------"<<std::endl;
    lot = 0;
    for(it = buffer_map.GetFreeBatches().cbegin() ; it != buffer_map.GetFreeBatches().cend() ; ++it)
    {
        cout<<"Batch "<<lot<<" starts at "<<(*it).Start()<<" with "<<(*it).Count()<<" string(s)"<<endl;
        lot++;
    }
    cout<<"--------------------------------"<<endl;

    return 0;
}
