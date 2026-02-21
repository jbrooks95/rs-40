#pragma once

#define _USE_MATH_DEFINES
#include "BiquadFilter.h"
#include "RC4558Model.h"
#include <cmath>

class InputStage
{
public:
    InputStage() = default;

    void prepare(double sampleRate)
    {
        sr = sampleRate;
        hpFilter.reset();
        opAmp.prepare(sampleRate);
        updateFilters();
    }

    void reset()
    {
        hpFilter.reset();
    }

    
    void setHiLevel(bool isHiLevel)
    {
        hiLevelMode = isHiLevel;
        updateFilters();
    }

    
    void setGain(double gainParam)
    {
        
        gainParam = std::clamp(gainParam, 0.0, 1.0);

        
        
        
        double potResistance = 10000.0 * std::pow(gainParam, 2.0); 

        
        
        
        
        
        double analogGain = 1.0 + potResistance / 100.0;

        
        if (hiLevelMode)
        {
            analogGain *= hiLevelAttenuation;
        }

        currentGain = analogGain;
        opAmp.setGain(analogGain);
        updateFilters();
    }

    
    void setSaturation(double amount)
    {
        opAmp.setSaturation(amount);
    }

    
    double process(double input)
    {
        
        double signal = opAmp.process(input);

        
        signal = hpFilter.process(signal);

        return signal;
    }

private:
    double sr = 44100.0;
    bool hiLevelMode = false;
    double currentGain = 1.0;

    
    
    
    static constexpr double hiLevelAttenuation = 0.128;

    
    BiquadFilter hpFilter;

    
    RC4558Stage opAmp;

    void updateFilters()
    {
        
        
        
        
        double effectiveR = 100.0; 
        double hpfCutoff = 1.0 / (2.0 * M_PI * effectiveR * 47.0e-6);

        hpFilter.setHighPass(sr, hpfCutoff, 0.707);
    }
};
