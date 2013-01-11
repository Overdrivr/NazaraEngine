#include <cmath>
#include "MyHeightSource.hpp"

MyHeightSource::MyHeightSource()
{
    m_source = new NzSimplex2D(15954);
    m_source2 = new NzFBM2D(SIMPLEX,15954);
}

MyHeightSource::~MyHeightSource()
{
    delete m_source;
    delete m_source2;
}

float MyHeightSource::GetNoiseValue(float x, float y)
{
    /*
    if(x > 100)
        return (x-100.f)*(x-100.f)/30000.f;
    else
        return 0.f;*/
/*
    if(x > 30)
        return (x-30.f)/100.f;
    else
        return 0.f;*/

    //return (std::sin(x/100.f)+1)/2.f;
    //return m_source->GetValue(x,y,0.005);
    //return m_source2->GetValue(x,y,0.0008)*5;
    return 0.f;
}
