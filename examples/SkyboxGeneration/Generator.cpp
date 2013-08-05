#include "Generator.hpp"

Generator::Generator()
{
    srand(123456789);
    ambient = new NzFBM3D(SIMPLEX,1234567891);
    ambientRes = 1/500.f;

}

Generator::~Generator()
{
    delete ambient;
}

void Generator::Generate(NzImage& image)
{
    NzVector3f p;

    if(!ambientColormap.LoadFromFile("resources/ambientColormap.png"))
        return;

    float offset = static_cast<float>(tileSize);
    NzVector3f center(offset/2.f);

    //FRONT
    for(int i(0) ; i < tileSize ; ++i)
        for(int j(0) ; j < tileSize ; ++j)
        {
            p.x = 0.f;
            p.y = static_cast<float>(-j) + offset;
            p.z = static_cast<float>(-i) + offset;

            p = (p - center).Normalize();
            p *= offset * 0.5f;

            image.SetPixelColor(ComputePixelColor(p),i,j + tileSize);
        }

    //BACK
    for(int i(0) ; i < tileSize ; ++i)
        for(int j(0) ; j < tileSize ; ++j)
        {
            p.x = offset;
            p.y = static_cast<float>(-j) + offset;
            p.z = static_cast<float>(i);

            p = (p - center).Normalize();
            p *= offset * 0.5f;

            image.SetPixelColor(ComputePixelColor(p),i + 2 * tileSize,j + tileSize);
        }


    //BOTTOM
    for(int i(0) ; i < tileSize ; ++i)
        for(int j(0) ; j < tileSize ; ++j)
        {
            p.x = static_cast<float>(i);
            p.y = 0.f;
            p.z = static_cast<float>(j);

            p = (p - center).Normalize();
            p *= offset * 0.5f;

            image.SetPixelColor(ComputePixelColor(p),i + tileSize,j + 2 * tileSize);
        }

    //TOP
    for(int i(0) ; i < tileSize ; ++i)
        for(int j(0) ; j < tileSize ; ++j)
        {
            p.x = static_cast<float>(i);
            p.y = offset;
            p.z = static_cast<float>(-j) + offset;

            p = (p - center).Normalize();
            p *= offset * 0.5f;

            image.SetPixelColor(ComputePixelColor(p),i + tileSize,j);
        }

    //RIGHT
    for(int i(0) ; i < tileSize ; ++i)
        for(int j(0) ; j < tileSize ; ++j)
        {
            p.x = static_cast<float>(i);
            p.y = static_cast<float>(-j) + offset;
            p.z = 0.f;

            p = (p - center).Normalize();
            p *= offset * 0.5f;

            image.SetPixelColor(ComputePixelColor(p),i + tileSize,j + tileSize);
        }

    //LEFT
    for(int i(0) ; i < tileSize ; ++i)
        for(int j(0) ; j < tileSize ; ++j)
        {
            p.x = static_cast<float>(-i) + offset;
            p.y = static_cast<float>(-j) + offset;
            p.z = offset;

            p = (p - center).Normalize();
            p *= offset * 0.5f;

            image.SetPixelColor(ComputePixelColor(p),i + 3 * tileSize,j + tileSize);
        }
}

NzColor Generator::ComputePixelColor(const NzVector3f& p)
{
    float lsb, density;
    NzColor finalColor;

    // Ajouter un faible offset aléatoire ( +- 1 LSB) au calcul de bruit permet d'effacer
    // l'artéfact provoqué par un gradient très doux, à savoir des cassures très visibles
    // Il donne également un grain à l'image et la rend plus réaliste
    lsb = (rand() % 100 - 50)/(100.f * 15.f);

    // Calcul du bruit (un fbm3D), on ajuste l'intervalle à [0;1]
    density = ambient->GetValue(p.x,p.y,p.z,ambientRes) * 0.5 + 0.5 + lsb;

    // On se sert d'un gradient généré sous inkscape pour choisir une couleur associée à la densité
    colorLayers[0] = ambientColormap.GetPixelColor(0,static_cast<unsigned int>(density * 255.f));

    return colorLayers[0];
}
