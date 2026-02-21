#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include "DSP/RS40Channel.h"

class RS40AudioProcessor : public juce::AudioProcessor
{
public:
    RS40AudioProcessor()
        : AudioProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          parameters(*this, nullptr, juce::Identifier("RS40"),
              createParameterLayout())
    {
        gainParam = parameters.getRawParameterValue("gain");
        loGainParam = parameters.getRawParameterValue("loGain");
        loTuneParam = parameters.getRawParameterValue("loTune");
        midGainParam = parameters.getRawParameterValue("midGain");
        midTuneParam = parameters.getRawParameterValue("midTune");
        hiGainParam = parameters.getRawParameterValue("hiGain");
        hiTuneParam = parameters.getRawParameterValue("hiTune");
        distEnabledParam = parameters.getRawParameterValue("distEnabled");
        distBlendParam = parameters.getRawParameterValue("distBlend");
        distDriveParam = parameters.getRawParameterValue("distDrive");
        outputLevelParam = parameters.getRawParameterValue("outputLevel");
        analogCharParam = parameters.getRawParameterValue("analogCharacter");
    }

    ~RS40AudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        for (auto& ch : channels)
            ch.prepare(sampleRate);
    }

    void releaseResources() override
    {
        for (auto& ch : channels)
            ch.reset();
    }

    void processBlock(juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& ) override
    {
        juce::ScopedNoDenormals noDenormals;

        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();
        int numSamples = buffer.getNumSamples();
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, numSamples);
        updateParameters();
        int numChannelsToProcess = std::min((int)totalNumInputChannels, 2);
        for (int ch = 0; ch < numChannelsToProcess; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            channels[ch].processBlock(channelData, numSamples);
        }
        for (int ch = 0; ch < numChannelsToProcess; ++ch)
        {
            double peak = channels[ch].getPeakLevel();
            if (peak > peakLevel.load())
                peakLevel.store(peak);
            channels[ch].resetPeakLevel();

            double clip = channels[ch].getClipAmount();
            if (clip > clipAmount.load())
                clipAmount.store(clip);
            channels[ch].resetClipAmount();
        }
        if (numChannelsToProcess > 0)
        {
            const float* ch0 = buffer.getReadPointer(0);
            int writeIdx = scopeWriteIndex.load();
            for (int i = 0; i < numSamples; ++i)
            {
                scopeDecimCounter++;
                if (scopeDecimCounter >= scopeDecimFactor)
                {
                    scopeDecimCounter = 0;
                    scopeBuffer[writeIdx] = ch0[i];
                    writeIdx = (writeIdx + 1) % (int)scopeBuffer.size();
                }
            }
            scopeWriteIndex.store(writeIdx);
        }
    }

    const juce::String getName() const override { return "RS-40"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        auto state = parameters.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        if (xmlState != nullptr)
            if (xmlState->hasTagName(parameters.state.getType()))
                parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }

    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return parameters; }

    double getPeakLevel() const { return peakLevel.load(); }
    void resetPeakLevel() { peakLevel.store(0.0); }

    double getClipAmount() const { return clipAmount.load(); }
    void resetClipAmount() { clipAmount.store(0.0); }

    const std::array<float, 512>& getScopeBuffer() const { return scopeBuffer; }
    int getScopeWriteIndex() const { return scopeWriteIndex.load(); }

private:

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"gain", 1}, "Gain",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"loGain", 1}, "Lo Gain",
            juce::NormalisableRange<float>(-15.0f, 15.0f, 0.1f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"loTune", 1}, "Lo Tune",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"midGain", 1}, "Mid Gain",
            juce::NormalisableRange<float>(-15.0f, 15.0f, 0.1f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"midTune", 1}, "Mid Tune",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"hiGain", 1}, "Hi Gain",
            juce::NormalisableRange<float>(-15.0f, 15.0f, 0.1f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"hiTune", 1}, "Hi Tune",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"distEnabled", 1}, "Distortion",
            false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"distBlend", 1}, "Dist Blend",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"distDrive", 1}, "Dist Drive",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"outputLevel", 1}, "Output",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.75f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"analogCharacter", 1}, "Analog",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

        return { params.begin(), params.end() };
    }

    void updateParameters()
    {
        for (auto& ch : channels)
        {
            ch.setGain(*gainParam);

            ch.setLoGain(*loGainParam);
            ch.setLoTune(*loTuneParam);
            ch.setMidGain(*midGainParam);
            ch.setMidTune(*midTuneParam);
            ch.setHiGain(*hiGainParam);
            ch.setHiTune(*hiTuneParam);

            ch.setDistortionEnabled(*distEnabledParam > 0.5f);
            ch.setDistortionBlend(*distBlendParam);
            ch.setDistortionDrive(*distDriveParam);

            ch.setOutputLevel(*outputLevelParam);
            ch.setAnalogCharacter(*analogCharParam);
        }
    }

    juce::AudioProcessorValueTreeState parameters;
    RS40Channel channels[2];
    std::atomic<double> peakLevel { 0.0 };
    std::atomic<double> clipAmount { 0.0 };
    std::array<float, 512> scopeBuffer {};
    std::atomic<int> scopeWriteIndex { 0 };
    int scopeDecimCounter = 0;
    static constexpr int scopeDecimFactor = 8;
    std::atomic<float>* gainParam = nullptr;
    std::atomic<float>* loGainParam = nullptr;
    std::atomic<float>* loTuneParam = nullptr;
    std::atomic<float>* midGainParam = nullptr;
    std::atomic<float>* midTuneParam = nullptr;
    std::atomic<float>* hiGainParam = nullptr;
    std::atomic<float>* hiTuneParam = nullptr;
    std::atomic<float>* distEnabledParam = nullptr;
    std::atomic<float>* distBlendParam = nullptr;
    std::atomic<float>* distDriveParam = nullptr;
    std::atomic<float>* outputLevelParam = nullptr;
    std::atomic<float>* analogCharParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RS40AudioProcessor)
};
