//
//  Copyright © 2023 Oberon Day-West. All rights reserved.
//  This code has been referenced and adapted from Bristow-Johnson (2005), neotec (2007), Falco (2009) and Zolzer (2011).
//

#pragma once

#include <JuceHeader.h>

#include <vector>

namespace gravity {
    enum class FilterType
    {
        lowpass,
        highpass,
        allpass
    };

    class ButterFilter
    {
        FilterType filterType;

        double coefficientA0, coefficientA1, coefficientA2, coefficientB1, coefficientB2;

        double cutOffFrequency, qualityFactor;

        double sampleRate;

        std::vector<double> previousSamples1, previousSamples2;

    public:
        ButterFilter(double sampleRate, FilterType type);

        void prepare(const juce::dsp::ProcessSpec& spec);

        void setFilterParameters(double cutOffFrequency, double qualityFactor, FilterType filterType);

        double processFilter(double inputSample, int channelNumber);

        void updateSampleRate(double newSampleRate);

        void process(const juce::dsp::ProcessContextReplacing<float>& context);
    };

    class LinkwitzRFilter
    {
        FilterType filterType;

    public:
        ButterFilter lowPassFilter, highPassFilter, allPassFilter;

        LinkwitzRFilter(double sampleRate);

        void prepare(const juce::dsp::ProcessSpec& spec);

        void setCrossoverFrequency(double crossoverFrequency);

        double processFilter(double inputSample, int channelNumber);

        void setType(FilterType newType);

        void process(const juce::dsp::ProcessContextReplacing<float>& context);
    };
}
