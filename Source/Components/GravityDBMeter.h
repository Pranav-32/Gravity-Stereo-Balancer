#pragma once

#include <JuceHeader.h>

namespace gravity {
    class GravityDBMeter : public juce::Component
    {
    public:
        GravityDBMeter();
        ~GravityDBMeter() override;

        void paint(juce::Graphics& g) override;
        void updateMeter(float LMag, float RMag);
    private:
        float L = 0.0f, R = 0.0f;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GravityDBMeter)
    };
}
