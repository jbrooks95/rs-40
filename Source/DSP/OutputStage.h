#pragma once

#define _USE_MATH_DEFINES
#include "BiquadFilter.h"
#include "RC4558Model.h"
#include <cmath>

class OutputStage
{
public:
    enum class OutputMode
    {
        Line,       
        Mic,        
        StageHi,    
        StageLo     
    };

    OutputStage() = default;

    void prepare(double sampleRate)
    {
        sr = sampleRate;
        outputBuffer.prepare(sampleRate);
        outputBuffer.setGain(1.0);
        
        
        outputHPF.setHighPass(sr, 33.9, 0.707);

        
        
        
        bandwidthLPF.setLowPass(sr, 22000.0, 0.707);
    }

    void reset()
    {
        outputHPF.reset();
        bandwidthLPF.reset();
    }

    
    void setOutputMode(OutputMode mode)
    {
        outputMode = mode;
    }

    
    void setOutputLevel(double level)
    {
        outputLevel = std::clamp(level, 0.0, 1.0);
    }

    
    void setSaturation(double amount)
    {
        outputBuffer.setSaturation(amount);
    }

    
    double process(double input)
    {
        double signal = input;

        
        signal *= outputLevel;

        
        signal = outputBuffer.process(signal);

        
        switch (outputMode)
        {
            case OutputMode::Line:
                
                signal = bandwidthLPF.process(signal);
                break;

            case OutputMode::Mic:
                
                signal = outputHPF.process(signal);
                signal *= micLevelAttenuation;
                break;

            case OutputMode::StageHi:
                
                signal = bandwidthLPF.process(signal);
                break;

            case OutputMode::StageLo:
                
                
                
                signal *= padAttenuation;
                signal = bandwidthLPF.process(signal);
                break;
        }

        return signal;
    }

private:
    double sr = 44100.0;
    OutputMode outputMode = OutputMode::Line;
    double outputLevel = 1.0;

    
    static constexpr double padAttenuation = 1000.0 / (10000.0 + 1000.0); 

    
    
    
    static constexpr double micLevelAttenuation = 150.0 / 4700.0; 

    RC4558Stage outputBuffer;
    BiquadFilter outputHPF;
    BiquadFilter bandwidthLPF;
};
