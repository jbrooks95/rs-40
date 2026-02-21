#pragma once

#include "InputStage.h"
#include "EQSection.h"
#include "DiodeClipper.h"
#include "OutputStage.h"
#include "Oversampling.h"

class RS40Channel
{
public:
    RS40Channel() = default;

    void prepare(double sampleRate)
    {
        sr = sampleRate;

        inputStage.prepare(sampleRate);
        diodeClipper.prepare(sampleRate);
        eqSection.prepare(sampleRate);
        outputStage.prepare(sampleRate);

        
        oversampler.prepare(sampleRate, 2); 

        
        
        double peakTau = 0.022;
        peakReleaseCoeff = std::exp(-1.0 / (peakTau * sampleRate));
    }

    void reset()
    {
        inputStage.reset();
        eqSection.reset();
        diodeClipper.reset();
        outputStage.reset();
        oversampler.reset();
        peakEnvelope = 0.0;
    }

    
    
    
    

    
    void setInputLevel(bool hiLevel) { inputStage.setHiLevel(hiLevel); }
    void setGain(double param) { inputStage.setGain(param); }

    
    void setDistortionEnabled(bool enabled) { diodeClipper.setEnabled(enabled); }
    void setDistortionBlend(double blend) { diodeClipper.setBlend(blend); }
    void setDistortionDrive(double drive) { diodeClipper.setDrive(drive); }

    
    void setLoGain(double db) { eqSection.setLoGain(db); }
    void setLoTune(double param) { eqSection.setLoTune(param); }
    void setMidGain(double db) { eqSection.setMidGain(db); }
    void setMidTune(double param) { eqSection.setMidTune(param); }
    void setMidQ(double q) { eqSection.setMidQ(q); }
    void setHiGain(double db) { eqSection.setHiGain(db); }
    void setHiTune(double param) { eqSection.setHiTune(param); }

    
    void setOutputMode(OutputStage::OutputMode mode) { outputStage.setOutputMode(mode); }
    void setOutputLevel(double level) { outputStage.setOutputLevel(level); }

    
    void setPreEQ(bool pre) { preEQ = pre; }

    
    void setAnalogCharacter(double amount)
    {
        inputStage.setSaturation(amount);
        eqSection.setSaturation(amount);
        outputStage.setSaturation(amount);
    }

    
    
    

    
    double processSample(double input)
    {
        
        double signal = inputStage.process(input);

        
        double peakBuss = std::abs(signal);

        
        if (diodeClipper.isActive())
        {
            double preClip = signal;
            signal = oversampler.process(signal, [this](double s) {
                return diodeClipper.process(s);
            });
            
            double absPre = std::abs(preClip);
            if (absPre > 0.001)
            {
                double reduction = std::abs(preClip - signal) / absPre;
                if (reduction > currentClipAmount)
                    currentClipAmount = reduction;
            }
        }

        
        peakBuss = std::max(peakBuss, std::abs(signal));

        
        double preEqSignal = signal;

        
        signal = eqSection.process(signal);

        
        peakBuss = std::max(peakBuss, std::abs(preEQ ? preEqSignal : signal));

        
        
        if (peakBuss > peakEnvelope)
            peakEnvelope = peakBuss;              
        else
            peakEnvelope *= peakReleaseCoeff;      

        
        if (peakEnvelope > currentPeakLevel)
            currentPeakLevel = peakEnvelope;

        
        double outputSignal = preEQ ? preEqSignal : signal;

        
        outputSignal = outputStage.process(outputSignal);

        return outputSignal;
    }

    
    void processBlock(float* buffer, int numSamples)
    {
        currentPeakLevel = 0.0;
        currentClipAmount = 0.0;

        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] = static_cast<float>(processSample(static_cast<double>(buffer[i])));
        }
    }

    
    double getPeakLevel() const { return currentPeakLevel; }
    void resetPeakLevel() { currentPeakLevel = 0.0; }

    double getClipAmount() const { return currentClipAmount; }
    void resetClipAmount() { currentClipAmount = 0.0; }

private:
    double sr = 44100.0;
    bool preEQ = false;

    
    InputStage inputStage;
    DiodeClipper diodeClipper;
    EQSection eqSection;
    OutputStage outputStage;
    Oversampler oversampler;

    
    double peakEnvelope = 0.0;         
    double peakReleaseCoeff = 0.999;   
    double currentPeakLevel = 0.0;     
    double currentClipAmount = 0.0;
};
