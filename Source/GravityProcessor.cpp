#include "GravityProcessor.h"
#include "GravityEditor.h"

GravityAudioProcessor::GravityAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Stereo Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Stereo Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "gravity_params", {
        std::make_unique<juce::AudioParameterFloat>("pre_gain", "Pre-Gain", juce::NormalisableRange<float> {-36.0f, 36.0f, 0.5f}, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("post_gain", "Post-Gain", juce::NormalisableRange<float> {-36.0f, 36.0f, 0.5f}, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("core_balance", "Core Balance", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f),
        std::make_unique<juce::AudioParameterInt>("core_air_crossover", "Core-Air Crossover", 5000, 20000, 10000),
        std::make_unique<juce::AudioParameterFloat>("air_width", "Air Width", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f}, 0.0f)
        }),
    crossoverLP(getSampleRate()), crossoverHP(getSampleRate())
{
    parameters.addParameterListener("pre_gain", this);
    parameters.addParameterListener("post_gain", this);
    parameters.addParameterListener("core_balance", this);
    parameters.addParameterListener("core_air_crossover", this);
    parameters.addParameterListener("air_width", this);

    preGainProcessor.setGainDecibels(parameters.getRawParameterValue("pre_gain")->load());
    postGainProcessor.setGainDecibels(parameters.getRawParameterValue("post_gain")->load());
    coreBalanceVal = parameters.getRawParameterValue("core_balance")->load();
    crossoverFreq = parameters.getRawParameterValue("core_air_crossover")->load();
    airWidthVal = parameters.getRawParameterValue("air_width")->load();

    crossoverLP.setType(gravity::FilterType::lowpass);
    crossoverHP.setType(gravity::FilterType::highpass);
}

GravityAudioProcessor::~GravityAudioProcessor()
{
    parameters.removeParameterListener("pre_gain", this);
    parameters.removeParameterListener("post_gain", this);
    parameters.removeParameterListener("core_balance", this);
    parameters.removeParameterListener("core_air_crossover", this);
    parameters.removeParameterListener("air_width", this);
}

void GravityAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    preGainProcessor.prepare(spec);
    postGainProcessor.prepare(spec);

    crossoverLP.prepare(spec);
    crossoverHP.prepare(spec);

    auto buffers = { &sideBuffer, &lowMidBuffer, &highMidBuffer };
    for (auto* b : buffers)
    {
        b->setSize(spec.numChannels , samplesPerBlock);
        b->clear();
    }

    smoothLGain.reset(sampleRate, 0.025);

    smoothRGain.reset(sampleRate, 0.025);

    smoothSideRatio.reset(sampleRate, 0.005);
    smoothAirWidth.reset(sampleRate, 0.005);
}

void GravityAudioProcessor::releaseResources()
{
    preGainProcessor.reset();
    postGainProcessor.reset();
}

bool GravityAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void GravityAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    if (numSamples <= 0) return;

    juce::dsp::AudioBlock<float> mainBlock{ buffer };

    preGainProcessor.process(juce::dsp::ProcessContextReplacing<float>(mainBlock));
    preGainLeft.store(buffer.getMagnitude(0, 0, numSamples));
    preGainRight.store(buffer.getMagnitude(1, 0, numSamples));

    float lFullMag = buffer.getMagnitude(0, 0, numSamples) + 0.0001f;
    float rFullMag = buffer.getMagnitude(1, 0, numSamples) + 0.0001f;
    float fullAvg = (lFullMag + rFullMag) * 0.5f;

    float lCorrection = juce::jlimit(0.25f, 4.0f, fullAvg / lFullMag);
    float rCorrection = juce::jlimit(0.25f, 4.0f, fullAvg / rFullMag);

    smoothLGain.setTargetValue(1.0f + coreBalanceVal * (lCorrection - 1.0f));
    smoothRGain.setTargetValue(1.0f + coreBalanceVal * (rCorrection - 1.0f));

    smoothSideRatio.setTargetValue(juce::jmap(coreBalanceVal, 1.0f, lCorrection / (rCorrection + 0.0001f)));
    smoothAirWidth.setTargetValue(airWidthVal);

    for (int i = 0; i < numSamples; ++i)
    {
        float l = buffer.getSample(0, i);
        float r = buffer.getSample(1, i);
        float s = (l - r) * 0.5f;

        float currentSideBalance = smoothSideRatio.getNextValue();
        float currentWidth = smoothAirWidth.getNextValue();

        sideBuffer.setSample(0, i, s * currentSideBalance * currentWidth);
    }

    lowMidBuffer = buffer;
    highMidBuffer = buffer;

    crossoverLP.setCrossoverFrequency(crossoverFreq);
    crossoverHP.setCrossoverFrequency(crossoverFreq);

    juce::dsp::AudioBlock<float> lodMidBlock(lowMidBuffer);
    juce::dsp::AudioBlock<float> highMidBlock(highMidBuffer);

    crossoverLP.process(juce::dsp::ProcessContextReplacing<float>(lodMidBlock));
    crossoverHP.process(juce::dsp::ProcessContextReplacing<float>(highMidBlock));

    for (int i = 0; i < numSamples; ++i)
    {
        float coreL = lowMidBuffer.getSample(0, i);
        float coreR = lowMidBuffer.getSample(1, i);
        float airL = highMidBuffer.getSample(0, i);
        float airR = highMidBuffer.getSample(1, i);
        float s = sideBuffer.getSample(0, i);

        float currentLGain = smoothLGain.getNextValue();
        float currentRGain = smoothRGain.getNextValue();

        buffer.setSample(0, i, (coreL * currentLGain) + airL + s);
        buffer.setSample(1, i, (coreR * currentRGain) + airR - s);
    }

    postGainProcessor.process(juce::dsp::ProcessContextReplacing<float>(mainBlock));
    postGainLeft.store(buffer.getMagnitude(0, 0, numSamples));
    postGainRight.store(buffer.getMagnitude(1, 0, numSamples));
}


bool GravityAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* GravityAudioProcessor::createEditor()
{
    return new GravityAudioProcessorEditor (*this);
}

void GravityAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void GravityAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessorValueTreeState& GravityAudioProcessor::getVts()
{
    return parameters;
}

void GravityAudioProcessor::parameterChanged(const juce::String& paramID, float newVal)
{
    if (paramID.equalsIgnoreCase("pre_gain"))
        preGainProcessor.setGainDecibels(newVal);
    else if (paramID.equalsIgnoreCase("post_gain"))
        postGainProcessor.setGainDecibels(newVal);
    else if (paramID == "core_balance") coreBalanceVal = newVal;
    else if (paramID == "core_air_crossover") crossoverFreq = newVal;
    else if (paramID == "air_width") airWidthVal = newVal;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GravityAudioProcessor();
}

const juce::String GravityAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GravityAudioProcessor::acceptsMidi() const
{
    return false;
}

bool GravityAudioProcessor::producesMidi() const
{
    return false;
}

bool GravityAudioProcessor::isMidiEffect() const
{
    return false;
}

double GravityAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GravityAudioProcessor::getNumPrograms()
{
    return 1;
}

int GravityAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GravityAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GravityAudioProcessor::getProgramName(int index)
{
    return {};
}

void GravityAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}