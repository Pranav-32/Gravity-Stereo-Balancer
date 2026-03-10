#pragma once

#include <JuceHeader.h>
#include "Utils/GravityFilters.h"

class GravityAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    GravityAudioProcessor();
    ~GravityAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getVts();

    float getPreGainLeft() const { return preGainLeft.load(); }
    float getPreGainRight() const { return preGainRight.load(); }
    float getPostGainLeft() const { return postGainLeft.load(); }
    float getPostGainRight() const { return postGainRight.load(); }
private:
    void parameterChanged(const juce::String& paramID, float newVal) override;
    juce::AudioProcessorValueTreeState parameters;

    juce::dsp::Gain<float> preGainProcessor;
    juce::dsp::Gain<float> postGainProcessor;

    std::atomic<float> preGainLeft;
    std::atomic<float> preGainRight;
    std::atomic<float> postGainLeft;
    std::atomic<float> postGainRight;

    juce::AudioBuffer<float> sideBuffer;
    juce::AudioBuffer<float> lowMidBuffer;
    juce::AudioBuffer<float> highMidBuffer;

    gravity::LinkwitzRFilter crossoverLP;
    gravity::LinkwitzRFilter crossoverHP;

    float coreBalanceVal{ 0.0f };
    float crossoverFreq{ 10000.0f };
    float airWidthVal{ 1.0f };

    juce::LinearSmoothedValue<float> smoothLGain{ 1.0f };
    juce::LinearSmoothedValue<float> smoothRGain{ 1.0f };

    juce::LinearSmoothedValue<float> smoothSideRatio{ 1.0f };
    juce::LinearSmoothedValue<float> smoothAirWidth{ 1.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GravityAudioProcessor)
};
