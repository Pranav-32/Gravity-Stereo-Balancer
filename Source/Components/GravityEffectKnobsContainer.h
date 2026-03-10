#pragma once

#include <JuceHeader.h>

#include "GravitySlider.h"

namespace gravity {
    class GravityEffectKnobsContainer : public juce::Component
    {
    public:
        GravityEffectKnobsContainer(juce::AudioProcessorValueTreeState& vts);
        ~GravityEffectKnobsContainer() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        gravity::GravitySlider& getCoreBalanceSlider() { return coreBalanceSlider; }
        gravity::GravitySlider& getCrossoverSlider() { return crossoverSlider; }
        gravity::GravitySlider& getAirWidthSlider() { return airWidthSlider; }
    private:
        gravity::GravitySlider coreBalanceSlider;
        gravity::GravitySlider crossoverSlider;
        gravity::GravitySlider airWidthSlider;

        juce::Rectangle<float> balanceArea;
        juce::Rectangle<float> crossoverArea;
        juce::Rectangle<float> airWidthArea;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GravityEffectKnobsContainer)
    };
}
