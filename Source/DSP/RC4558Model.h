#pragma once

#include <algorithm>
#include <cmath>

class RC4558Model
{
public:
    RC4558Model() = default;

    void prepare(double sampleRate)
    {
        sr = sampleRate;
        
        
        
        
        
        
        updateBandwidthFilter();
    }

    
    void setGain(double newGain)
    {
        gain = std::max(1.0, newGain);
        updateBandwidthFilter();
    }

    
    void setSaturationAmount(double amount)
    {
        saturationAmount = std::clamp(amount, 0.0, 1.0);
    }

    
    double process(double input)
    {
        
        double signal = input * gain;

        
        
        
        
        if (saturationAmount > 0.001)
        {
            signal = applySaturation(signal);
        }

        
        signal = applyBandwidthLimit(signal);

        return signal;
    }

private:
    double sr = 44100.0;
    double gain = 1.0;
    double saturationAmount = 0.7;  

    
    
    
    
    static constexpr double supplyRail = 13.5;     
    static constexpr double normalLevel = 5.0;      
    static constexpr double headroomRatio = supplyRail / normalLevel; 

    
    double bwCoeff = 1.0;
    double bwState = 0.0;

    
    double applySaturation(double x) const
    {
        
        double scaled = x / headroomRatio;

        
        double saturated = headroomRatio * std::tanh(scaled);

        
        
        
        double asymmetry = 0.05 * saturationAmount;
        double asymmetricTerm = asymmetry * (1.0 - std::tanh(scaled * scaled));

        
        double result = x * (1.0 - saturationAmount) + 
                        (saturated + asymmetricTerm) * saturationAmount;

        return result;
    }

    
    void updateBandwidthFilter()
    {
        
        constexpr double gbwHz = 3000000.0; 
        double bandwidthHz = gbwHz / gain;

        
        bandwidthHz = std::min(bandwidthHz, sr * 0.49);

        
        double wc = 2.0 * M_PI * bandwidthHz / sr;
        bwCoeff = wc / (wc + 1.0);
    }

    double applyBandwidthLimit(double input)
    {
        bwState += bwCoeff * (input - bwState);
        return bwState;
    }
};

class RC4558Stage
{
public:
    void prepare(double sampleRate)
    {
        model.prepare(sampleRate);
    }

    void setGain(double g)
    {
        model.setGain(g);
    }

    void setSaturation(double s)
    {
        model.setSaturationAmount(s);
    }

    double process(double input)
    {
        return model.process(input);
    }

    void reset()
    {
        model.prepare(44100.0); 
    }

private:
    RC4558Model model;
};
