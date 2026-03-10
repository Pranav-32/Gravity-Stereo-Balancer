#pragma once

#include <JuceHeader.h>
#include "GravityProcessor.h"

#include "Components/GravityHeader.h"
#include "Components/GravityDBMeter.h"
#include "Components/GravityVisualContent.h"
#include "Components/GravityEffectKnobsContainer.h"

namespace gravity {
    class GlobalLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        GlobalLookAndFeel() {}
        
        juce::Typeface::Ptr getTypefaceForFont(const juce::Font& font) override {
            return juce::Typeface::createSystemTypefaceFor(BinaryData::BBHSansBartle_ttf, BinaryData::BBHSansBartle_ttfSize);
        }
    };
}

class GravityAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    GravityAudioProcessorEditor (GravityAudioProcessor&);
    ~GravityAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
private:
    GravityAudioProcessor& audioProcessor;

    gravity::GlobalLookAndFeel globalLNF;

    gravity::GravityHeader header;

    gravity::GravityDBMeter inputDBMeter;
    gravity::GravityDBMeter outputDBMeter;

    gravity::GravityEffectKnobsContainer fxKnobsCtnr;

    gravity::GravityVisualContent visualContent;

    gravity::GravitySlider preGainSlider;
    gravity::GravitySlider postGainSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GravityAudioProcessorEditor)
};
