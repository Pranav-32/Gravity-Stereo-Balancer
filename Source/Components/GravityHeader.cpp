#include "GravityHeader.h"

#include "../Colors.h"

gravity::GravityHeader::GravityHeader()
{
}

gravity::GravityHeader::~GravityHeader()
{
}

void gravity::GravityHeader::paint(juce::Graphics& g)
{
    std::unique_ptr<juce::Drawable> svgDrawable;

    if (auto drb = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::Gravity_Logo_svg).get())) {
        drb->drawWithin(g, juce::Rectangle<float>(getWidth() * 0.5f - 20.0f, 5.0f, 40.0f, 40.0f), juce::RectanglePlacement::fillDestination, 1.0f);
    }

    juce::Font titleOutlineFont(27.0f, juce::Font::bold);
    juce::Font titleFont(25.0f, juce::Font::bold);

    g.setFont(titleOutlineFont);
    g.setColour(gravity::colors::accent);
    g.drawText("GRAVITY", 0, 0, getWidth(), getHeight(), juce::Justification::centred);

    g.setFont(titleFont);
    g.setColour(juce::Colours::white);
    g.drawText("GRAVITY", 0, 0, getWidth(), getHeight(), juce::Justification::centred);

    g.setColour(gravity::colors::secondary_background);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
    g.fillRect(0, 0, 1, getHeight());
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
}
