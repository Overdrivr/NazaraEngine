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
        NzFBM3D* deepAmbient;
        float ambientRes;
        float deepAmbientRes;
        std::array<NzColor,12> colorLayers;
        std::array<float,12> layersWeight;
        NzImage ambientColormap;
        NzImage deepAmbientColormap;
};

#endif // GENERATOR_HPP
