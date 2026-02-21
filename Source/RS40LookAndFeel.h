#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
class PeakLED : public juce::Component
{
public:
    PeakLED()
    {
        setInterceptsMouseClicks(false, false);
        setPaintingIsUnclipped(true);
    }

    void setActive(bool shouldBeActive)
    {
        if (active != shouldBeActive)
        {
            active = shouldBeActive;
            repaint();
        }
    }

    bool isActive() const { return active; }

    void paint(juce::Graphics& g) override
    {
        auto fullBounds = getLocalBounds().toFloat();
        auto centre = fullBounds.getCentre();

        
        float ledRadius = juce::jmin(fullBounds.getWidth(), fullBounds.getHeight()) * 0.25f;
        auto ledBounds = juce::Rectangle<float>(
            centre.x - ledRadius, centre.y - ledRadius,
            ledRadius * 2.0f, ledRadius * 2.0f);

        if (active)
        {
            
            float glowRadius = juce::jmin(fullBounds.getWidth(), fullBounds.getHeight()) * 0.5f;
            juce::ColourGradient glow(
                juce::Colour(0xFF8B1A2B).withAlpha(0.4f), centre.x, centre.y,
                juce::Colour(0xFF8B1A2B).withAlpha(0.0f), centre.x + glowRadius, centre.y,
                true);
            g.setGradientFill(glow);
            g.fillEllipse(fullBounds);

            
            g.setColour(juce::Colour(0xFFCC2244));
            g.fillEllipse(ledBounds);

            
            g.setColour(juce::Colours::white.withAlpha(0.25f));
            auto highlight = ledBounds.reduced(ledRadius * 0.4f);
            highlight.setY(ledBounds.getY() + ledRadius * 0.15f);
            highlight.setHeight(ledRadius * 0.6f);
            g.fillEllipse(highlight);
        }
        else
        {
            
            g.setColour(juce::Colour(0xFF3A1518));
            g.fillEllipse(ledBounds);

            
            g.setColour(juce::Colours::white.withAlpha(0.06f));
            auto highlight = ledBounds.reduced(ledRadius * 0.4f);
            highlight.setY(ledBounds.getY() + ledRadius * 0.15f);
            highlight.setHeight(ledRadius * 0.6f);
            g.fillEllipse(highlight);
        }

        
        g.setColour(juce::Colour(0xFF1E1A17));
        g.drawEllipse(ledBounds, 1.0f);
    }

private:
    bool active = false;
};
class RS40LookAndFeel : public juce::LookAndFeel_V4
{
public:
    RS40LookAndFeel()
    {
        setColour(juce::Slider::textBoxTextColourId, accentColor);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::textColourId, textColor.withAlpha(0.7f));
        setColour(juce::ToggleButton::textColourId, textColor);
        setColour(juce::ToggleButton::tickColourId, accentColor);
    }

    void setBrandFont(const juce::Font& font) { brandFont = font; }

    
    
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        auto arcRadius = radius - 2.0f;
        auto knobRadius = radius * 0.68f;
        auto lineThickness = 3.0f;

        
        {
            auto tickRadius = arcRadius + 6.0f;
            int numTicks = 11;
            g.setColour(textColor.withAlpha(0.4f));
            for (int i = 0; i <= numTicks; ++i)
            {
                float tickAngle = rotaryStartAngle + (float)i / (float)numTicks * (rotaryEndAngle - rotaryStartAngle);
                float innerR = tickRadius - 4.0f;
                float outerR = tickRadius;
                float cosA = std::cos(tickAngle);
                float sinA = std::sin(tickAngle);
                g.drawLine(centreX + innerR * sinA, centreY - innerR * cosA,
                           centreX + outerR * sinA, centreY - outerR * cosA, 1.5f);
            }
        }

        
        {
            juce::Path bgArc;
            bgArc.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                                0.0f, rotaryStartAngle, rotaryEndAngle, true);
            g.setColour(mutedColor);
            g.strokePath(bgArc, juce::PathStrokeType(lineThickness,
                         juce::PathStrokeType::curved,
                         juce::PathStrokeType::rounded));
        }

        
        if (sliderPos > 0.0f)
        {
            juce::Path valueArc;
            valueArc.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                                   0.0f, rotaryStartAngle, toAngle, true);
            auto fillColour = slider.findColour(juce::Slider::rotarySliderFillColourId);
            g.setColour(fillColour);
            g.strokePath(valueArc, juce::PathStrokeType(lineThickness,
                         juce::PathStrokeType::curved,
                         juce::PathStrokeType::rounded));
        }

        
        {
            auto baseRadius = knobRadius + 4.0f;
            g.setColour(juce::Colour(0xFF0A0808));
            g.fillEllipse(centreX - baseRadius, centreY - baseRadius,
                          baseRadius * 2.0f, baseRadius * 2.0f);
            g.setColour(juce::Colour(0xFF1A1612));
            g.drawEllipse(centreX - baseRadius, centreY - baseRadius,
                          baseRadius * 2.0f, baseRadius * 2.0f, 1.0f);
        }

        
        {
            juce::ColourGradient metallic(
                juce::Colour(0xFF3A3530), centreX, centreY - knobRadius * 0.5f,
                juce::Colour(0xFF1E1A17), centreX, centreY + knobRadius,
                false);
            g.setGradientFill(metallic);
            g.fillEllipse(centreX - knobRadius, centreY - knobRadius,
                          knobRadius * 2.0f, knobRadius * 2.0f);

            
            g.setColour(juce::Colour(0xFF151210));
            g.drawEllipse(centreX - knobRadius, centreY - knobRadius,
                          knobRadius * 2.0f, knobRadius * 2.0f, 1.5f);
        }

        
        {
            int numKnurls = 36;
            auto knurlInner = knobRadius - 1.5f;
            auto knurlOuter = knobRadius + 0.5f;
            g.setColour(textColor.withAlpha(0.2f));
            for (int i = 0; i < numKnurls; ++i)
            {
                float angle = (float)i / (float)numKnurls * juce::MathConstants<float>::twoPi;
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);
                g.drawLine(centreX + knurlInner * cosA, centreY + knurlInner * sinA,
                           centreX + knurlOuter * cosA, centreY + knurlOuter * sinA, 0.8f);
            }
        }

        
        {
            juce::Path pointer;
            auto pointerThickness = 2.0f;
            auto innerRadius = knobRadius * 0.55f;
            auto outerRadius = knobRadius * 0.9f;
            pointer.addRoundedRectangle(-pointerThickness * 0.5f, -outerRadius,
                                        pointerThickness, outerRadius - innerRadius,
                                        1.0f);
            g.setColour(textColor);
            g.fillPath(pointer, juce::AffineTransform::rotation(toAngle)
                                    .translated(centreX, centreY));
        }
    }

    
    
    
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        bool isOn = button.getToggleState();

        
        if (isOn)
        {
            juce::ColourGradient glow(
                accentColor.withAlpha(0.25f), bounds.getCentreX(), bounds.getCentreY(),
                accentColor.withAlpha(0.0f), bounds.getCentreX(), bounds.getY() - 6.0f,
                true);
            g.setGradientFill(glow);
            g.fillRoundedRectangle(bounds.expanded(4.0f), 6.0f);
        }

        
        auto baseColour = isOn ? accentColor : juce::Colour(0xFF2A2520);
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, 4.0f);

        
        g.setColour(isOn ? accentColor : mutedColor);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        
        auto textColour = isOn ? juce::Colour(0xFF0A0808) : textColor;
        g.setColour(textColour);
        if (brandFont != juce::Font())
            g.setFont(brandFont.withHeight(13.0f));
        else
            g.setFont(juce::Font(juce::FontOptions("Courier New", 13.0f, juce::Font::bold)));
        g.drawText(button.getButtonText(), bounds.toNearestInt(),
                   juce::Justification::centred);
    }

    
    
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll(label.findColour(juce::Label::backgroundColourId));

        if (!label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
            if (brandFont != juce::Font())
                g.setFont(brandFont.withHeight(11.0f));
            else
                g.setFont(juce::Font(juce::FontOptions("Courier New", 11.0f, juce::Font::bold)));

            auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
            g.drawFittedText(label.getText().toUpperCase(), textArea,
                             label.getJustificationType(),
                             juce::jmax(1, (int)((float)textArea.getHeight() / 11.0f)),
                             label.getMinimumHorizontalScale());
        }
    }

private:
    juce::Colour textColor     { 0xFFE8DCC8 };
    juce::Colour accentColor   { 0xFFD4912A };
    juce::Colour dimColor      { 0xFF9A8E7E };
    juce::Colour mutedColor    { 0xFF5E554A };
    juce::Colour panelColor    { 0xFF1E1A17 };
    juce::Colour highlightColor{ 0xFF8B1A2B };

    juce::Font brandFont;
};
