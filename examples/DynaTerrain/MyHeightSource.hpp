#ifndef MYHEIGHTSOURCE_HPP
#define MYHEIGHTSOURCE_HPP

#include <Nazara/Noise/FBM2D.hpp>
#include <Nazara/Noise/HybridMultiFractal2D.hpp>
#include <Nazara/Noise/Simplex2D.hpp>
#include <Nazara/Noise/Worley2D.hpp>
#include <Nazara/DynaTerrain/HeightSource.hpp>

class MyHeightSource : public NzHeightSource
{
    public:
        MyHeightSource();
        ~MyHeightSource();
        float GetNoiseValue(float x, float y);
    protected:
    private:
        NzSimplex2D* m_source;
        NzWorley2D m_worley;
        NzFBM2D* m_source2;
        NzHybridMultiFractal2D* m_source3;
        float resolution;
};

#endif // MYHEIGHTSOURCE_HPP
