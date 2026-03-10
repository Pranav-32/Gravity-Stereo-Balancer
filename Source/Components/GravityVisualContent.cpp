#include "GravityVisualContent.h"

#include "../Colors.h"

gravity::GravityVisualContent::GravityVisualContent(juce::AudioProcessorValueTreeState& vts)
{
	vts.addParameterListener("core_balance", this);
	vts.addParameterListener("air_width", this);
    vts.addParameterListener("core_air_crossover", this);

    coreBalanceVal = vts.getRawParameterValue("core_balance")->load();
    airWidthVal = vts.getRawParameterValue("air_width")->load();
    crossoverFreq = vts.getRawParameterValue("core_air_crossover")->load();
}

gravity::GravityVisualContent::~GravityVisualContent()
{
}

void gravity::GravityVisualContent::deregisterApvtsListeners(juce::AudioProcessorValueTreeState& vts)
{
    vts.removeParameterListener("core_balance", this);
    vts.removeParameterListener("air_width", this);
    vts.removeParameterListener("core_air_crossover", this);
}

void gravity::GravityVisualContent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    auto center = area.getCentre();
    const float width = area.getWidth();
    const float height = area.getHeight();

    const float minFreq = 20.0f;
    const float maxFreq = 20000.0f;

    float freqNormalized = (std::log10(juce::jlimit(minFreq, maxFreq, crossoverFreq)) - std::log10(minFreq))
        / (std::log10(maxFreq) - std::log10(minFreq));

    float maxPossibleRadius = std::max(width, height) * 0.5f;
    float influenceRadius = maxPossibleRadius * freqNormalized;

    g.setColour(gravity::colors::accent.withAlpha(0.25f));
    g.drawEllipse(center.x - influenceRadius, center.y - influenceRadius,
        influenceRadius * 2.0f, influenceRadius * 2.0f, 1.0f);

    const float gravityWellDiameter = height * 0.35f;
    const float haloBase = height * 0.15f;
    const float innerHaloSize = gravityWellDiameter + (haloBase * juce::jlimit(0.0f, 0.5f, airWidthVal));
    const float outerHaloSize = gravityWellDiameter + (haloBase * airWidthVal);
    const float safeRadius = (outerHaloSize * 0.5f) + 4.0f;

    for (const auto& p : particles) {
        float dx = center.x - p.gridPos.x;
        float dy = center.y - p.gridPos.y;
        float distFromOrigin = std::sqrt(dx * dx + dy * dy);

        float pullStrength = 0.0f;
        if (distFromOrigin <= influenceRadius) {
            float proximity = 1.0f - (distFromOrigin / influenceRadius);
            float alignment = std::max(std::abs(dx), std::abs(dy)) / (distFromOrigin + 0.001f);
            pullStrength = coreBalanceVal * proximity * alignment;
        }

        for (int i = 0; i < 4; ++i) {
            float followerLag = (float)i / 4.0f;
            float individualPull = pullStrength * (1.0f - (followerLag * 0.7f));

            auto currentX = p.gridPos.x + (dx * individualPull);
            auto currentY = p.gridPos.y + (dy * individualPull);

            float distDx = currentX - center.x;
            float distDy = currentY - center.y;
            float currentDist = std::sqrt(distDx * distDx + distDy * distDy);

            if (currentDist < safeRadius && currentDist > 0.01f) {
                float scale = safeRadius / currentDist;
                currentX = center.x + (distDx * scale);
                currentY = center.y + (distDy * scale);
            }

            float baseAlpha = (distFromOrigin > influenceRadius) ? 0.15f : 0.4f;
            float alpha = juce::jlimit(0.0f, 1.0f, (baseAlpha + (pullStrength * 0.5f)) * (1.0f - (followerLag * 0.4f)));

            g.setColour(gravity::colors::accent.withAlpha(alpha));
            float s = p.size * (1.0f - (followerLag * 0.3f));
            g.fillEllipse(currentX - (s * 0.5f), currentY - (s * 0.5f), s, s);
        }
    }

    if (airWidthVal > 0.5f) {
        g.setColour(gravity::colors::accent.withAlpha(0.25f));
        g.fillEllipse(juce::Rectangle<float>(outerHaloSize, outerHaloSize).withCentre(center));
    }
    g.setColour(gravity::colors::accent.withAlpha(0.45f));
    g.fillEllipse(juce::Rectangle<float>(innerHaloSize, innerHaloSize).withCentre(center));
    g.setColour(gravity::colors::accent);
    g.fillEllipse(juce::Rectangle<float>(gravityWellDiameter, gravityWellDiameter).withCentre(center));

    g.setColour(gravity::colors::secondary_background);
    g.fillRect(0.0f, 0.0f, 1.0f, height);
    g.fillRect(width - 1.0f, 0.0f, 1.0f, height);
}

void gravity::GravityVisualContent::resized()
{
    particles.clear();
    auto bounds = getLocalBounds().toFloat();

    int cols = 35;
    int rows = 25;
    float stepX = bounds.getWidth() / (float)cols;
    float stepY = bounds.getHeight() / (float)rows;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            Particle p;
            p.gridPos = { (x * stepX) + (stepX * 0.5f), (y * stepY) + (stepY * 0.5f) };
            p.size = 3.0f;
            particles.push_back(p);
        }
    }
}


void gravity::GravityVisualContent::parameterChanged(const juce::String& parameterID, float newValue)
{
	if (parameterID.equalsIgnoreCase("core_balance"))
		coreBalanceVal = newValue;
	if (parameterID.equalsIgnoreCase("air_width"))
		airWidthVal = newValue;
    if (parameterID.equalsIgnoreCase("core_air_crossover"))
        crossoverFreq = newValue;
	repaint();
}


void gravity::GravityVisualContent::initParticles() {
	particles.clear();
	auto bounds = getLocalBounds().toFloat();
	int cols = 10;
	int rows = 10;

	for (int i = 0; i < numParticles; ++i) {
		float x = (i % cols) * (bounds.getWidth() / cols) + (bounds.getWidth() / cols * 0.5f);
		float y = (i / cols) * (bounds.getHeight() / rows) + (bounds.getHeight() / rows * 0.5f);
		particles.push_back({ {x, y}, juce::Random::getSystemRandom().nextFloat() * 2.0f + 1.0f });
	}
}

