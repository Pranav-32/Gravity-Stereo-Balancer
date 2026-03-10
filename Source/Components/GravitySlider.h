#pragma once

#include <JuceHeader.h>

namespace gravity {
    class GravitySliderLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPosProportional, float rotaryStartAngle,
            float rotaryEndAngle, juce::Slider& slider) override;
    };

    class GravitySlider : public juce::Component {
    public:
        GravitySlider(juce::AudioProcessorValueTreeState& vts, const juce::String& paramID);
        ~GravitySlider() override;
        
        void resized() override;

        void mouseEnter(const juce::MouseEvent& event) override;
        void mouseExit(const juce::MouseEvent& event) override;

        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override;
        void mouseUp(const juce::MouseEvent& event) override;
        void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

        bool isHovered = false;

        juce::Slider& getJuceSlider() { return slider; }
    private:
        GravitySliderLookAndFeel lnf;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

        float defaultValue = 0.0f;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GravitySlider)
    };
}
