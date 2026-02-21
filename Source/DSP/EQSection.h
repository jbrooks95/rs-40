#pragma once

#define _USE_MATH_DEFINES
#include "BiquadFilter.h"
#include "RC4558Model.h"
#include <cmath>
#include <algorithm>

class EQSection
{
public:
    EQSection() = default;

    void prepare(double sampleRate)
    {
        sr = sampleRate;
        loFilter.reset();
        midFilter.reset();
        hiFilter.reset();
        
        
        loOpAmp.prepare(sampleRate);
        midOpAmp.prepare(sampleRate);
        hiOpAmp.prepare(sampleRate);
        sumOpAmp.prepare(sampleRate);

        
        loOpAmp.setGain(2.0);
        midOpAmp.setGain(2.0);
        hiOpAmp.setGain(2.0);
        sumOpAmp.setGain(1.0);

        updateAllFilters();
    }

    void reset()
    {
        loFilter.reset();
        midFilter.reset();
        hiFilter.reset();
    }

    
    
    

    
    void setLoGain(double gainDb)
    {
        loGainDb = std::clamp(gainDb, -15.0, 15.0);
        updateLoFilter();
    }

    
    void setLoTune(double tuneParam)
    {
        tuneParam = std::clamp(tuneParam, 0.0, 1.0);
        
        loFreqHz = loFreqMin * std::pow(loFreqMax / loFreqMin, tuneParam);
        updateLoFilter();
    }

    
    
    

    
    void setMidGain(double gainDb)
    {
        midGainDb = std::clamp(gainDb, -15.0, 15.0);
        updateMidFilter();
    }

    
    void setMidTune(double tuneParam)
    {
        tuneParam = std::clamp(tuneParam, 0.0, 1.0);
        
        double logParam = std::pow(tuneParam, 2.0); 
        midFreqHz = midFreqMin * std::pow(midFreqMax / midFreqMin, logParam);
        updateMidFilter();
    }

    
    void setMidQ(double q)
    {
        midQ = std::clamp(q, 0.3, 5.0);
        updateMidFilter();
    }

    
    
    

    
    void setHiGain(double gainDb)
    {
        hiGainDb = std::clamp(gainDb, -15.0, 15.0);
        updateHiFilter();
    }

    
    void setHiTune(double tuneParam)
    {
        tuneParam = std::clamp(tuneParam, 0.0, 1.0);
        hiFreqHz = hiFreqMin * std::pow(hiFreqMax / hiFreqMin, tuneParam);
        updateHiFilter();
    }

    
    
    

    void setSaturation(double amount)
    {
        double s = std::clamp(amount, 0.0, 1.0);
        loOpAmp.setSaturation(s);
        midOpAmp.setSaturation(s);
        hiOpAmp.setSaturation(s);
        sumOpAmp.setSaturation(s);
    }

    
    
    

    
    double process(double input)
    {
        
        
        
        double signal = input;

        
        signal = loFilter.process(signal);

        
        signal = midFilter.process(signal);

        
        signal = hiFilter.process(signal);

        
        signal = sumOpAmp.process(signal);

        return signal;
    }

    
    double getLoFreq() const { return loFreqHz; }
    double getMidFreq() const { return midFreqHz; }
    double getHiFreq() const { return hiFreqHz; }

private:
    double sr = 44100.0;

    
    static constexpr double loFreqMin = 30.0;   
    static constexpr double loFreqMax = 300.0;   
    double loFreqHz = 95.0;
    double loGainDb = 0.0;
    BiquadFilter loFilter;
    RC4558Stage loOpAmp;

    
    static constexpr double midFreqMin = 160.0;  
    static constexpr double midFreqMax = 8000.0;  
    double midFreqHz = 1130.0;
    double midGainDb = 0.0;
    double midQ = 1.2;  
    BiquadFilter midFilter;
    RC4558Stage midOpAmp;

    
    static constexpr double hiFreqMin = 700.0;   
    static constexpr double hiFreqMax = 8000.0;   
    double hiFreqHz = 2370.0;
    double hiGainDb = 0.0;
    BiquadFilter hiFilter;
    RC4558Stage hiOpAmp;

    
    RC4558Stage sumOpAmp;

    void updateLoFilter()
    {
        
        
        loFilter.setLowShelf(sr, loFreqHz, loGainDb, 0.6);
    }

    void updateMidFilter()
    {
        midFilter.setPeaking(sr, midFreqHz, midGainDb, midQ);
    }

    void updateHiFilter()
    {
        
        hiFilter.setHighShelf(sr, hiFreqHz, hiGainDb, 0.6);
    }

    void updateAllFilters()
    {
        updateLoFilter();
        updateMidFilter();
        updateHiFilter();
    }
};
