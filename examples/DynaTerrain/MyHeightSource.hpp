#ifndef MYHEIGHTSOURCE_HPP
#define MYHEIGHTSOURCE_HPP

#include <Nazara/Noise/FBM2D.hpp>
#include <Nazara/Noise/Simplex2D.hpp>
//#include <Nazara/DynaTerrain/HeightSource.hpp>
#include "HeightSource.hpp"

class MyHeightSource : public NzHeightSource
{
    public:
        MyHeightSource();
        ~MyHeightSource();
        float GetNoiseValue(float x, float y);
    protected:
    private:
        NzSimplex2D* m_source;
        NzFBM2D* m_source2;
};

#endif // MYHEIGHTSOURCE_HPP
