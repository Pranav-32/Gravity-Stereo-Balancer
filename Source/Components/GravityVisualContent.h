#pragma once

#include <JuceHeader.h>

namespace gravity {
    class GravityVisualContent : public juce::Component, public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        GravityVisualContent(juce::AudioProcessorValueTreeState& vts);
        ~GravityVisualContent() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void deregisterApvtsListeners(juce::AudioProcessorValueTreeState& vts);
    private:
        float coreBalanceVal = 0.0f, airWidthVal = 0.0f;
        float crossoverFreq = 10000.0f;

        struct Particle {
            juce::Point<float> gridPos;
            float size;
        };

        static constexpr int numParticles = 100;
        std::vector<Particle> particles;
        void initParticles();

        void parameterChanged(const juce::String& parameterID, float newValue) override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GravityVisualContent)
    };
}