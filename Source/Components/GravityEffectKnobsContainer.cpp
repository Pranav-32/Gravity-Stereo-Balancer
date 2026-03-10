#include "GravityEffectKnobsContainer.h"
#include "../Colors.h"

gravity::GravityEffectKnobsContainer::GravityEffectKnobsContainer(juce::AudioProcessorValueTreeState& vts) :
	coreBalanceSlider(vts, "core_balance"),
	crossoverSlider(vts, "core_air_crossover"),
	airWidthSlider(vts, "air_width")
{
	addAndMakeVisible(coreBalanceSlider);
	addAndMakeVisible(crossoverSlider);
	addAndMakeVisible(airWidthSlider);
}

gravity::GravityEffectKnobsContainer::~GravityEffectKnobsContainer()
{
}

void gravity::GravityEffectKnobsContainer::paint(juce::Graphics& g)
{
    g.fillAll(gravity::colors::secondary_background);

    g.setColour(gravity::colors::text);
    g.setFont(juce::Font(12.0f, juce::Font::bold));

    const float labelY = getHeight() - 25.0f;
    const float labelHeight = 15.0f;

    g.drawText("CORE BALANCE", balanceArea.withY(balanceArea.getBottom() - 5.0f).withHeight(labelHeight),
        juce::Justification::centred);

    g.drawText("AIR WIDTH", airWidthArea.withY(airWidthArea.getBottom() - 5.0f).withHeight(labelHeight),
        juce::Justification::centred);

    g.setFont(juce::Font(10.0f, juce::Font::bold));

    g.drawText("CORE-AIR X-OVER", crossoverArea.withY(crossoverArea.getBottom() - 5.0f).withHeight(labelHeight).expanded(juce::Font(12.0f, juce::Font::bold).getStringWidthFloat("CORE-AIR X-OVER") - crossoverArea.getWidth(), 0.0f),
        juce::Justification::centred);
}


void gravity::GravityEffectKnobsContainer::resized()
{
    auto bounds = getLocalBounds().toFloat().reduced(5.0f);
    auto sliderArea = bounds.removeFromTop(bounds.getHeight() - 20.0f).withTrimmedTop(5.0f);
    float unit = sliderArea.getWidth() / 2.5f;

    coreBalanceSlider.setBounds(sliderArea.removeFromLeft(unit).toNearestInt());

    auto middleBounds = sliderArea.removeFromLeft(unit * 0.5f);
    crossoverSlider.setBounds(middleBounds.reduced(5.0f, 15.0f).toNearestInt());

    airWidthSlider.setBounds(sliderArea.toNearestInt());

    balanceArea = coreBalanceSlider.getBounds().toFloat();
    crossoverArea = crossoverSlider.getBounds().toFloat();
    airWidthArea = airWidthSlider.getBounds().toFloat();
}
