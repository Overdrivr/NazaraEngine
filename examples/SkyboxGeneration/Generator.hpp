#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <Nazara/Noise/FBM3D.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Core/Color.hpp>

const unsigned int tileSize = 512;

class Generator
{
    public:
        Generator();
        void Generate(NzImage& image);
        NzColor ComputePixelColor(const NzVector3f& p);
        ~Generator();
    protected:
    private:
        NzFBM3D* ambient;
        float ambientRes;
        std::array<NzColor,12> colorLayers;
        NzImage ambientColormap;
};

#endif // GENERATOR_HPP
