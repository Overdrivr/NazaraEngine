#include <iostream>
#include <Nazara/Core/String.hpp>
//#include <Nazara/Utility/StackArray2D.hpp>
#include <Nazara/DynaTerrain/StackArray2D.hpp>
using namespace std;

int main()
{
     ///Exemple d'application du StackArray2D
     //Comme son nom l'indique, il permet de contenir un empilement de tableau � 2 dimensions ou bien un tableau � 3 dimensions
     //De plus, la structure m�moire interne (une std::map) permet d'avoir un tableau rempli uniquement � certains endroits
     //sans gaspiller de m�moire pour les coordonn�es vides
     //Il est �galement possible de savoir si une coordon�e existe ou non, c'est � dire si elle contient ou non une valeur

     //Cette structure est moins efficace qu'une tableau classique pour un acc�s s�quentiel, mais pr�sente une consommation m�moire
     //moins importante pour des donn�es relativement �parpill�es.

     //Cette structure est utile notamment pour contenir des arbres de donn�es non �quilibr�s (certaines sont bien plus profondes que d'autres)

    StackArray2D<NzString> tab;
    //Au point de hauteur 2 et
    tab.at(2,1,0) = "Hello !";

    cout<<"Exists ? "<<tab.Exists(2,2,1)<<"|"<<tab.Exists(2,0,1)<<"|"<<tab.Exists(2,1,1)<<"|"<<tab.Exists(0,2,0)<<endl;
    cout<<"Value ? "<<tab.at(0,2,0)<<endl;
    cout<<"Value ? "<<tab.at(2,1,0)<<endl;


    return 0;
}
