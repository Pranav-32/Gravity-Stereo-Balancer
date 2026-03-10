#include "GravityProcessor.h"
#include "GravityEditor.h"

#include "Colors.h"

GravityAudioProcessorEditor::GravityAudioProcessorEditor (GravityAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    preGainSlider(audioProcessor.getVts(), "pre_gain"),
    postGainSlider(audioProcessor.getVts(), "post_gain"),
    fxKnobsCtnr(audioProcessor.getVts()),
    visualContent(audioProcessor.getVts())
{
    startTimerHz(60);

    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);

    setSize (700, 450);

    addAndMakeVisible(header);

    addAndMakeVisible(inputDBMeter);
    addAndMakeVisible(outputDBMeter);

    addAndMakeVisible(fxKnobsCtnr);

    addAndMakeVisible(visualContent);

    addAndMakeVisible(preGainSlider);
    addAndMakeVisible(postGainSlider);

}

GravityAudioProcessorEditor::~GravityAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    visualContent.deregisterApvtsListeners(audioProcessor.getVts());
}

void GravityAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(gravity::colors::background);

    g.setColour(gravity::colors::text);
    g.setFont(juce::Font(10.0f, juce::Font::bold));

    auto preGainArea = preGainSlider.getBounds().toFloat();
    g.drawText("PRE-GAIN",
        preGainArea.withY(preGainArea.getBottom() - 5.0f).withHeight(15.0f).expanded(20, 0),
        juce::Justification::centred);

    auto postGainArea = postGainSlider.getBounds().toFloat();
    g.drawText("POST-GAIN",
        postGainArea.withY(postGainArea.getBottom() - 5.0f).withHeight(15.0f).expanded(20, 0),
        juce::Justification::centred);
}


void GravityAudioProcessorEditor::resized()
{
    header.setBounds(120, 0, 460, 50);
    inputDBMeter.setBounds(0, 0, 120, 380);
    outputDBMeter.setBounds(580, 0, 120, 380);

    preGainSlider.setBounds(35, 375, 50, 50);
    postGainSlider.setBounds(615, 375, 50, 50);

    fxKnobsCtnr.setBounds(120, 335, 460, 115);
    visualContent.setBounds(120, 50, 460, 285);
}

void GravityAudioProcessorEditor::timerCallback()
{
    inputDBMeter.updateMeter(audioProcessor.getPreGainLeft(), audioProcessor.getPreGainRight());
    outputDBMeter.updateMeter(audioProcessor.getPostGainLeft(), audioProcessor.getPostGainRight());
}
