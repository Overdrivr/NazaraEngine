#include <cmath>
#include "MyHeightSource.hpp"

MyHeightSource::MyHeightSource()
{
    //m_source = new NzFBM2D<T>(SIMPLEX,15954);
}

MyHeightSource::~MyHeightSource()
{
    //delete m_source;
}

float MyHeightSource::GetNoiseValue(float x, float y)
{
    return (std::sin(x/1000.f)+1);
    //return source->GetValue(x,y,30);
}
