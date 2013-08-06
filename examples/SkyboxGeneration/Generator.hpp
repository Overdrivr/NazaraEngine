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

        void BlendLayers(NzImage& finalImage);

        //Les générateurs pour chaque couche
        void ComputeAmbientColor(unsigned x, unsigned y, const NzVector3f& p);
        void ComputeForStars(const NzVector3f& p);

        void DrawSingleStar(unsigned int x, unsigned int y, float innerRadius, float falloffRadius, float falloff = 0.5);

        void Generate(NzImage& image);

        ~Generator();
    protected:
    private:
        NzFBM3D* ambient;
        unsigned int imgx, imgy;

        NzImage ambientColormap;
        NzImage deepAmbientColormap;

        NzImage ambientLayer;
        NzImage starLayer;
};

#endif // GENERATOR_HPP
