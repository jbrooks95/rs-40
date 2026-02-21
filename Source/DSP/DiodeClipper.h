#pragma once

#include <cmath>
#include <algorithm>

class DiodeClipper
{
public:
    DiodeClipper() = default;

    void prepare(double sampleRate)
    {
        sr = sampleRate;
    }

    void reset()
    {
        prevOutput = 0.0;
    }

    
    bool isActive() const
    {
        return enabled && blend > 0.001;
    }

    
    void setEnabled(bool shouldBeEnabled)
    {
        enabled = shouldBeEnabled;
    }

    
    void setBlend(double newBlend)
    {
        blend = std::clamp(newBlend, 0.0, 1.0);
    }

    
    void setDrive(double newDrive)
    {
        driveGain = std::clamp(newDrive, 0.1, 10.0);
    }

    
    double process(double input)
    {
        if (!enabled || blend < 0.001)
            return input;

        double driven = input * driveGain;

        
        double vOut = solveNR(driven);

        
        vOut /= driveGain;

        
        double output = input * (1.0 - blend) + vOut * blend;

        prevOutput = output;
        return output;
    }

private:
    double sr = 44100.0;
    bool enabled = false;
    double blend = 1.0;
    double driveGain = 1.0;
    double prevOutput = 0.0;

    
    static constexpr double Is = 2.52e-9;      
    static constexpr double nDiode = 1.752;     
    static constexpr double Vt = 0.02585;       
    static constexpr double nVt = nDiode * Vt;  

    
    
    
    
    static constexpr double seriesR = 1000.0;   

    
    static constexpr int maxIterations = 8;
    static constexpr double tolerance = 1e-6;

    
    static double diodeCurrent(double v)
    {
        
        double vClamped = std::clamp(v, -1.5, 1.5);
        return 2.0 * Is * std::sinh(vClamped / nVt);
    }

    
    static double diodeCurrentDerivative(double v)
    {
        double vClamped = std::clamp(v, -1.5, 1.5);
        return (2.0 * Is / nVt) * std::cosh(vClamped / nVt);
    }

    
    double solveNR(double vin) const
    {
        
        double vOut = std::clamp(vin, -1.2, 1.2);

        for (int i = 0; i < maxIterations; ++i)
        {
            double iDiode = diodeCurrent(vOut);
            double diDv = diodeCurrentDerivative(vOut);

            double f = vOut + seriesR * iDiode - vin;
            double fPrime = 1.0 + seriesR * diDv;

            double step = f / fPrime;
            vOut -= step;

            if (std::abs(step) < tolerance)
                break;
        }

        return vOut;
    }
};
