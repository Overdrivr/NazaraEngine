#include <iostream>
#include <Nazara/Core/String.hpp>
//#include <Nazara/Utility/StackArray2D.hpp>
#include <Nazara/DynaTerrain/StackArray2D.hpp>
using namespace std;

int main()
{
     ///Exemple d'application du StackArray2D
     //Comme son nom l'indique, il permet de contenir un empilement de tableau à 2 dimensions ou bien un tableau à 3 dimensions
     //De plus, la structure mémoire interne (une std::map) permet d'avoir un tableau rempli uniquement à certains endroits
     //sans gaspiller de mémoire pour les coordonnées vides
     //Il est également possible de savoir si une coordonée existe ou non, c'est à dire si elle contient ou non une valeur

     //Cette structure est moins efficace qu'une tableau classique pour un accès séquentiel, mais présente une consommation mémoire
     //moins importante pour des données relativement éparpillées.

     //Cette structure est utile notamment pour contenir des arbres de données non équilibrés (certaines sont bien plus profondes que d'autres)

    StackArray2D<NzString> tab;
    //Au point de hauteur 2 et
    tab.at(2,1,0) = "Hello !";

    cout<<"Exists ? "<<tab.Exists(2,2,1)<<"|"<<tab.Exists(2,0,1)<<"|"<<tab.Exists(2,1,1)<<"|"<<tab.Exists(0,2,0)<<endl;
    cout<<"Value ? "<<tab.at(0,2,0)<<endl;
    cout<<"Value ? "<<tab.at(2,1,0)<<endl;


    return 0;
}
