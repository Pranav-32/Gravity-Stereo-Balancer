#include "GravitySlider.h"

#include "../Colors.h"

void gravity::GravitySliderLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    const float radius = (float)juce::jmin(width / 2, height / 2);
    const float centerX = (float)x + (float)width * 0.5f;
    const float centerY = (float)y + (float)height * 0.5f;
    const float thickness = radius * 0.35f;

    float startAngle = juce::degreesToRadians(-45.0f - 90.0f);
    constexpr float endAngle = juce::degreesToRadians(225.0f - 90.0f);
    const float valAngle = startAngle + sliderPosProportional * (endAngle - startAngle);

    juce::Path bgArc;
    bgArc.addCentredArc(centerX, centerY, radius * 0.825f, radius * 0.825f, 0.0f, startAngle, endAngle, true);

    g.setColour(gravity::colors::empty);
    g.strokePath(bgArc, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));

    if (slider.getMinimum() < 0 && slider.getMaximum() > 0) startAngle = juce::degreesToRadians(0.0f);

    juce::Path sliderArc;
    sliderArc.addCentredArc(centerX, centerY, radius * 0.825f, radius * 0.825f, 0.0f, startAngle, valAngle, true);

    g.setColour(gravity::colors::accent);
    g.strokePath(sliderArc, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));

    g.setColour(gravity::colors::knob);
    g.fillEllipse(centerX - radius * 0.65f, centerY - radius * 0.65f, radius * 0.65f * 2.0f, radius * 0.65f * 2.0f);

    juce::Point<float> knobLineEnd = {
        centerX + std::sin(valAngle) * radius * 0.65f,
        centerY - std::cos(valAngle) * radius * 0.65f
    };

    g.setColour(gravity::colors::accent);
    g.drawLine(juce::Line<float>({ centerX, centerY }, knobLineEnd), 2.0f);
}


gravity::GravitySlider::GravitySlider(juce::AudioProcessorValueTreeState& vts, const juce::String& paramID)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setLookAndFeel(&lnf);

    slider.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(slider);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, paramID, slider);

    if (auto* param = vts.getParameter(paramID))
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            defaultValue = rangedParam->getDefaultValue();

            defaultValue = rangedParam->getNormalisableRange().convertFrom0to1(defaultValue);
        }
    }
}

gravity::GravitySlider::~GravitySlider()
{
    slider.setLookAndFeel(nullptr);

}

void gravity::GravitySlider::resized()
{
    slider.setBounds(getLocalBounds());
}

void gravity::GravitySlider::mouseEnter(const juce::MouseEvent& event)
{
    isHovered = true;
}

void gravity::GravitySlider::mouseExit(const juce::MouseEvent& event)
{
    isHovered = false;
}

void gravity::GravitySlider::mouseDown(const juce::MouseEvent& event)
{
    auto center = getLocalBounds().getCentre();
    auto cursorPos = event.position;
    const float cursorDistance = std::sqrt(std::pow(cursorPos.x - center.x, 2) + std::pow(cursorPos.y - center.y, 2));

    const float radius = juce::jmin(getWidth(), getHeight()) * 0.5f;

    if (cursorDistance > radius) return;

    if (event.mods.isCtrlDown() && event.mods.isLeftButtonDown() || event.mods.isMiddleButtonDown()) {
        slider.setValue(defaultValue, juce::sendNotification);
        return;
    }

    slider.mouseDown(event.getEventRelativeTo(&slider));
}

void gravity::GravitySlider::mouseDrag(const juce::MouseEvent& event)
{
    slider.mouseDrag(event.getEventRelativeTo(&slider));
}

void gravity::GravitySlider::mouseUp(const juce::MouseEvent& event)
{
    slider.mouseUp(event.getEventRelativeTo(&slider));
}

void gravity::GravitySlider::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    slider.mouseWheelMove(event.getEventRelativeTo(&slider), wheel);

}
