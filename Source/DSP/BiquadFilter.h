#pragma once

#include <cmath>
#include <algorithm>

class BiquadFilter
{
public:
    BiquadFilter() { reset(); }

    void reset()
    {
        z1 = 0.0;
        z2 = 0.0;
    }

    
    
    double process(double input)
    {
        double output = b0 * input + z1;
        z1 = b1 * input - a1 * output + z2;
        z2 = b2 * input - a2 * output;
        return output;
    }

    
    
    
    
    

    
    void setLowShelf(double sampleRate, double freqHz, double gainDb, double Q = 0.707)
    {
        double A = std::pow(10.0, gainDb / 40.0);
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double alpha = sinw0 / (2.0 * Q);
        double sqrtA = std::sqrt(A);

        double a0 = (A + 1.0) + (A - 1.0) * cosw0 + 2.0 * sqrtA * alpha;
        b0 = (A * ((A + 1.0) - (A - 1.0) * cosw0 + 2.0 * sqrtA * alpha)) / a0;
        b1 = (2.0 * A * ((A - 1.0) - (A + 1.0) * cosw0)) / a0;
        b2 = (A * ((A + 1.0) - (A - 1.0) * cosw0 - 2.0 * sqrtA * alpha)) / a0;
        a1 = (-2.0 * ((A - 1.0) + (A + 1.0) * cosw0)) / a0;
        a2 = ((A + 1.0) + (A - 1.0) * cosw0 - 2.0 * sqrtA * alpha) / a0;
    }

    
    void setHighShelf(double sampleRate, double freqHz, double gainDb, double Q = 0.707)
    {
        double A = std::pow(10.0, gainDb / 40.0);
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double alpha = sinw0 / (2.0 * Q);
        double sqrtA = std::sqrt(A);

        double a0 = (A + 1.0) - (A - 1.0) * cosw0 + 2.0 * sqrtA * alpha;
        b0 = (A * ((A + 1.0) + (A - 1.0) * cosw0 + 2.0 * sqrtA * alpha)) / a0;
        b1 = (-2.0 * A * ((A - 1.0) + (A + 1.0) * cosw0)) / a0;
        b2 = (A * ((A + 1.0) + (A - 1.0) * cosw0 - 2.0 * sqrtA * alpha)) / a0;
        a1 = (2.0 * ((A - 1.0) - (A + 1.0) * cosw0)) / a0;
        a2 = ((A + 1.0) - (A - 1.0) * cosw0 - 2.0 * sqrtA * alpha) / a0;
    }

    
    void setPeaking(double sampleRate, double freqHz, double gainDb, double Q = 1.0)
    {
        if (std::abs(gainDb) < 0.01)
        {
            
            b0 = 1.0; b1 = 0.0; b2 = 0.0;
            a1 = 0.0; a2 = 0.0;
            return;
        }

        double A = std::pow(10.0, gainDb / 40.0);
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double alpha = sinw0 / (2.0 * Q);

        double a0 = 1.0 + alpha / A;
        b0 = (1.0 + alpha * A) / a0;
        b1 = (-2.0 * cosw0) / a0;
        b2 = (1.0 - alpha * A) / a0;
        a1 = (-2.0 * cosw0) / a0;
        a2 = (1.0 - alpha / A) / a0;
    }

    
    void setHighPass(double sampleRate, double freqHz, double Q = 0.707)
    {
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double alpha = sinw0 / (2.0 * Q);

        double a0 = 1.0 + alpha;
        b0 = ((1.0 + cosw0) / 2.0) / a0;
        b1 = (-(1.0 + cosw0)) / a0;
        b2 = ((1.0 + cosw0) / 2.0) / a0;
        a1 = (-2.0 * cosw0) / a0;
        a2 = (1.0 - alpha) / a0;
    }

    
    void setLowPass(double sampleRate, double freqHz, double Q = 0.707)
    {
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double alpha = sinw0 / (2.0 * Q);

        double a0 = 1.0 + alpha;
        b0 = ((1.0 - cosw0) / 2.0) / a0;
        b1 = (1.0 - cosw0) / a0;
        b2 = ((1.0 - cosw0) / 2.0) / a0;
        a1 = (-2.0 * cosw0) / a0;
        a2 = (1.0 - alpha) / a0;
    }

private:
    
    double b0 = 1.0, b1 = 0.0, b2 = 0.0;
    double a1 = 0.0, a2 = 0.0;

    
    double z1 = 0.0;
    double z2 = 0.0;
};
