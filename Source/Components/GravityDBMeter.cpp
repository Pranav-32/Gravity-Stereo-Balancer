#include "GravityDBMeter.h"

#include "../Colors.h"

gravity::GravityDBMeter::GravityDBMeter()
{
}

gravity::GravityDBMeter::~GravityDBMeter()
{
}

void gravity::GravityDBMeter::paint (juce::Graphics& g)
{
	auto bounds = getLocalBounds().toFloat().reduced(10.0f);
	auto leftCol = bounds.removeFromLeft(bounds.getWidth() * 0.5f).reduced(5.0f);
	auto rightCol = bounds.reduced(5.0f);

	g.setColour(gravity::colors::empty);
	g.fillRect(leftCol);
	g.fillRect(rightCol);

	g.setColour(gravity::colors::accent);

	auto leftFill = leftCol.reduced(5.0f);
	auto rightFill = rightCol.reduced(5.0f);

	g.fillRect(leftFill.withTop(leftFill.getBottom() - (leftFill.getHeight() * juce::jlimit(0.0f, 1.0f, L))));
	g.fillRect(rightFill.withTop(rightFill.getBottom() - (rightFill.getHeight() * juce::jlimit(0.0f, 1.0f, R))));

}

void gravity::GravityDBMeter::updateMeter(float LMag, float RMag)
{
	L = LMag;
	R = RMag;
	repaint();
}
