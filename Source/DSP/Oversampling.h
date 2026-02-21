#pragma once

#include "BiquadFilter.h"
#include <vector>
#include <functional>

class Oversampler
{
public:
    Oversampler() = default;

    
    void prepare(double sampleRate, int factor = 2)
    {
        baseSampleRate = sampleRate;
        oversamplingFactor = std::clamp(factor, 1, 4);
        oversampledRate = sampleRate * oversamplingFactor;

        
        
        
        double cutoff = sampleRate * 0.45; 

        for (auto& f : upsampleFilters)
            f.setLowPass(oversampledRate, cutoff, 0.54); 

        for (auto& f : downsampleFilters)
            f.setLowPass(oversampledRate, cutoff, 0.54);

        reset();
    }

    void reset()
    {
        for (auto& f : upsampleFilters) f.reset();
        for (auto& f : downsampleFilters) f.reset();
    }

    
    double getOversampledRate() const { return oversampledRate; }
    int getFactor() const { return oversamplingFactor; }

    
    double process(double input, const std::function<double(double)>& processFunc)
    {
        if (oversamplingFactor == 1)
            return processFunc(input);

        double result = 0.0;

        for (int i = 0; i < oversamplingFactor; ++i)
        {
            
            double upsampled = (i == 0) ? input * oversamplingFactor : 0.0;

            
            for (auto& f : upsampleFilters)
                upsampled = f.process(upsampled);

            
            double processed = processFunc(upsampled);

            
            for (auto& f : downsampleFilters)
                processed = f.process(processed);

            
            if (i == 0)
                result = processed;
        }

        return result;
    }

private:
    double baseSampleRate = 44100.0;
    double oversampledRate = 88200.0;
    int oversamplingFactor = 2;

    
    
    BiquadFilter upsampleFilters[2];
    BiquadFilter downsampleFilters[2];
};
