#include "GravityFilters.h"
#include <numbers>

gravity::ButterFilter::ButterFilter(double sampleRate, FilterType type) : filterType(type),
sampleRate(sampleRate),
previousSamples1(2, 0),
previousSamples2(2, 0)
{
}

void gravity::ButterFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    previousSamples1.resize(spec.numChannels, 0);
    previousSamples2.resize(spec.numChannels, 0);

    std::fill(previousSamples1.begin(), previousSamples1.end(), 0.0);
    std::fill(previousSamples2.begin(), previousSamples2.end(), 0.0);
}

void gravity::ButterFilter::setFilterParameters(double cutOffFrequency, double qualityFactor, FilterType filterType)
{
    this->cutOffFrequency = cutOffFrequency;
    this->qualityFactor = qualityFactor;
    this->filterType = filterType;

    double w0 = 2 * std::numbers::pi * cutOffFrequency / sampleRate;
    double alpha = std::sin(w0) / (2 * qualityFactor);

    if (filterType == FilterType::lowpass)
    {
        double a0 = 1 + alpha;
        coefficientA0 = (1 - std::cos(w0)) / 2 / a0;
        coefficientA1 = (1 - std::cos(w0)) / a0;
        coefficientA2 = coefficientA0;
        coefficientB1 = -2 * std::cos(w0) / a0;
        coefficientB2 = (1 - alpha) / a0;
    }
    else if (filterType == FilterType::highpass)
    {
        double a0 = 1 + alpha;
        coefficientA0 = (1 + std::cos(w0)) / 2 / a0;
        coefficientA1 = -(1 + std::cos(w0)) / a0;
        coefficientA2 = coefficientA0;
        coefficientB1 = -2 * std::cos(w0) / a0;
        coefficientB2 = (1 - alpha) / a0;
    }
    else if (filterType == FilterType::allpass)
    {
        double a0 = 1 + alpha;
        coefficientA0 = (1 - alpha) / a0;
        coefficientA1 = -2 * std::cos(w0) / a0;
        coefficientA2 = (1 + alpha) / a0;
        coefficientB1 = coefficientA1;
        coefficientB2 = coefficientA0;
    }
    else
    {
        throw std::invalid_argument("Invalid filter type.");
    }
}

double gravity::ButterFilter::processFilter(double inputSample, int channelNumber)
{
    if (channelNumber < 0 || channelNumber >= previousSamples1.size())
    {
        throw std::out_of_range("Invalid channel index.");
    }

    double outputSample = coefficientA0 * inputSample + coefficientA1 * previousSamples1[channelNumber] + coefficientA2 * previousSamples2[channelNumber] - coefficientB1 * previousSamples1[channelNumber] - coefficientB2 * previousSamples2[channelNumber];

    previousSamples2[channelNumber] = previousSamples1[channelNumber];
    previousSamples1[channelNumber] = inputSample;
    previousSamples1[channelNumber] = outputSample;

    return outputSample;
}


void gravity::ButterFilter::updateSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    setFilterParameters(cutOffFrequency, qualityFactor, filterType);
}

void gravity::ButterFilter::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const int numChannels = inputBlock.getNumChannels();
    const int numSamples = inputBlock.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            outputBlock.getChannelPointer(channel)[i] = processFilter(inputBlock.getChannelPointer(channel)[i], channel);
        }
    }
}

gravity::LinkwitzRFilter::LinkwitzRFilter(double sampleRate) : lowPassFilter(sampleRate,
    FilterType::lowpass),
    highPassFilter(sampleRate,
        FilterType::highpass),
    allPassFilter(sampleRate,
        FilterType::allpass)
{
}

void gravity::LinkwitzRFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);
    allPassFilter.prepare(spec);
}

void gravity::LinkwitzRFilter::setType(FilterType newType)
{
    filterType = newType;
}

void gravity::LinkwitzRFilter::setCrossoverFrequency(double crossoverFrequency)
{
    lowPassFilter.setFilterParameters(crossoverFrequency, 0.707, FilterType::lowpass);
    highPassFilter.setFilterParameters(crossoverFrequency, 0.707, FilterType::highpass);
    allPassFilter.setFilterParameters(crossoverFrequency, 0.707, FilterType::allpass);
}

double gravity::LinkwitzRFilter::processFilter(double inputSample, int channelNumber)
{
    if (filterType == FilterType::lowpass)
    {
        return lowPassFilter.processFilter(lowPassFilter.processFilter(inputSample, channelNumber), channelNumber);
    }
    else if (filterType == FilterType::highpass)
    {
        return highPassFilter.processFilter(highPassFilter.processFilter(inputSample, channelNumber), channelNumber);
    }
    else
    {
        return allPassFilter.processFilter(allPassFilter.processFilter(inputSample, channelNumber), channelNumber);
    }
    return 0.0;
}

void gravity::LinkwitzRFilter::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const int numChannels = inputBlock.getNumChannels();
    const int numSamples = inputBlock.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            outputBlock.getChannelPointer(channel)[i] = processFilter(inputBlock.getChannelPointer(channel)[i], channel);
        }
    }
}