#pragma once

#include <JuceHeader.h>

namespace gravity {
    class GravityHeader : public juce::Component
    {
    public:
        GravityHeader();
        ~GravityHeader() override;

        void paint(juce::Graphics& g) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GravityHeader)
    };
}
