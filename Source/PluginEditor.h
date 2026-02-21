#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "RS40LookAndFeel.h"
#include "BinaryData.h"
class DiodeSchematic : public juce::Component, private juce::Timer
{
public:
    DiodeSchematic() { setInterceptsMouseClicks(false, false); }

    void setDistEnabled(bool enabled)
    {
        if (enabled == targetState) return;
        targetState = enabled;
        sweepProgress = 0.0f;
        startTimerHz(60);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        auto panelColor = juce::Colour(0xFF1E1A17);
        auto mutedCol = juce::Colour(0xFF5E554A);
        auto amberCol = juce::Colour(0xFFD4912A);
        auto crimsonCol = juce::Colour(0xFF8B1A2B);
        auto textColor = juce::Colour(0xFF9A8E7E);
        g.setColour(panelColor);
        g.fillRoundedRectangle(bounds, 3.0f);
        g.setColour(mutedCol.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        float h = bounds.getHeight();
        float wireY = cy - h * 0.12f;
        float groundY = cy + h * 0.28f;
        float junctionX = cx;
        float wireLeft = bounds.getX() + 20.0f;
        float wireRight = bounds.getRight() - 20.0f;
        float diodeSize = std::min(h * 0.18f, 14.0f);
        float wireThick = 1.5f;
        bool animating = isTimerRunning();
        float sweepX = wireLeft + sweepProgress * (wireRight - wireLeft);
        auto onWire = amberCol.withAlpha(0.6f);
        auto offWire = mutedCol;
        auto newWireCol = targetState ? onWire : offWire;
        auto oldWireCol = targetState ? offWire : onWire;
        auto newDiodeCol = targetState ? crimsonCol : mutedCol;
        auto oldDiodeCol = targetState ? mutedCol : crimsonCol;
        auto newArrowCol = targetState ? amberCol.withAlpha(0.4f) : mutedCol.withAlpha(0.3f);
        auto oldArrowCol = targetState ? mutedCol.withAlpha(0.3f) : amberCol.withAlpha(0.4f);
        if (!animating)
        {
            newWireCol = targetState ? onWire : offWire;
            oldWireCol = newWireCol;
            newDiodeCol = targetState ? crimsonCol : mutedCol;
            oldDiodeCol = newDiodeCol;
            newArrowCol = targetState ? amberCol.withAlpha(0.4f) : mutedCol.withAlpha(0.3f);
            oldArrowCol = newArrowCol;
        }
        auto colorAt = [&](float x, juce::Colour swept, juce::Colour unswept) {
            return animating ? (x <= sweepX ? swept : unswept) : swept;
        };
        if (animating && sweepX > wireLeft && sweepX < wireRight)
        {
            g.setColour(newWireCol);
            g.drawLine(wireLeft, wireY, sweepX, wireY, wireThick);
            g.setColour(oldWireCol);
            g.drawLine(sweepX, wireY, wireRight, wireY, wireThick);
        }
        else
        {
            g.setColour(newWireCol);
            g.drawLine(wireLeft, wireY, wireRight, wireY, wireThick);
        }
        g.setColour(colorAt(junctionX, newWireCol, oldWireCol));
        g.fillEllipse(junctionX - 2.5f, wireY - 2.5f, 5.0f, 5.0f);
        g.setColour(colorAt(junctionX, newWireCol, oldWireCol));
        g.drawLine(junctionX, wireY, junctionX, groundY, wireThick);
        {
            float dTop = wireY + 4.0f;
            float dBot = dTop + diodeSize;
            float dHalf = diodeSize * 0.55f;
            float dX = junctionX - diodeSize * 1.4f;

            auto wc = colorAt(dX, newWireCol, oldWireCol);
            auto dc = colorAt(dX, newDiodeCol, oldDiodeCol);

            g.setColour(wc);
            g.drawLine(dX, wireY, dX, dTop, wireThick);
            g.drawLine(dX, dBot, dX, groundY, wireThick);

            juce::Path tri;
            tri.addTriangle(dX - dHalf, dTop, dX + dHalf, dTop, dX, dBot);
            g.setColour(dc);
            g.fillPath(tri);

            g.setColour(wc);
            g.drawLine(dX - dHalf, dBot, dX + dHalf, dBot, 2.0f);
        }
        {
            float dBot = groundY - 4.0f;
            float dTop = dBot - diodeSize;
            float dHalf = diodeSize * 0.55f;
            float dX = junctionX + diodeSize * 1.4f;

            auto wc = colorAt(dX, newWireCol, oldWireCol);
            auto dc = colorAt(dX, newDiodeCol, oldDiodeCol);

            g.setColour(wc);
            g.drawLine(dX, wireY, dX, dTop, wireThick);
            g.drawLine(dX, dBot, dX, groundY, wireThick);

            juce::Path tri;
            tri.addTriangle(dX - dHalf, dBot, dX + dHalf, dBot, dX, dTop);
            g.setColour(dc);
            g.fillPath(tri);

            g.setColour(wc);
            g.drawLine(dX - dHalf, dTop, dX + dHalf, dTop, 2.0f);
        }
        {
            float gw1 = 14.0f, gw2 = 9.0f, gw3 = 4.0f;
            float gap = 3.0f;
            g.setColour(colorAt(junctionX, newWireCol, oldWireCol));
            g.drawLine(junctionX - gw1, groundY, junctionX + gw1, groundY, wireThick);
            g.drawLine(junctionX - gw2, groundY + gap, junctionX + gw2, groundY + gap, wireThick);
            g.drawLine(junctionX - gw3, groundY + gap * 2, junctionX + gw3, groundY + gap * 2, wireThick);
        }
        g.setColour(textColor.withAlpha(0.6f));
        auto font = juce::Font(juce::FontOptions("Courier New", 9.0f, juce::Font::plain));
        g.setFont(font);

        g.drawText("IN", (int)(wireLeft - 2), (int)(wireY - 14), 24, 12, juce::Justification::centred);
        g.drawText("OUT", (int)(wireRight - 18), (int)(wireY - 14), 28, 12, juce::Justification::centred);

        g.setFont(font.withHeight(8.0f));
        g.drawText("1N914", (int)(junctionX - 22), (int)(groundY + 12), 44, 10, juce::Justification::centred);
        float arrowY = wireY;
        {
            float ax = wireLeft + 24.0f;
            g.setColour(colorAt(ax, newArrowCol, oldArrowCol));
            juce::Path arrow;
            arrow.addTriangle(ax, arrowY - 3.5f, ax, arrowY + 3.5f, ax + 6.0f, arrowY);
            g.fillPath(arrow);
        }
        {
            float ax = wireRight - 30.0f;
            g.setColour(colorAt(ax, newArrowCol, oldArrowCol));
            juce::Path arrow;
            arrow.addTriangle(ax, arrowY - 3.5f, ax, arrowY + 3.5f, ax + 6.0f, arrowY);
            g.fillPath(arrow);
        }
    }

private:
    bool targetState = false;
    float sweepProgress = 1.0f;

    void timerCallback() override
    {
        sweepProgress += 1.0f / 18.0f;
        if (sweepProgress >= 1.0f)
        {
            sweepProgress = 1.0f;
            stopTimer();
        }
        repaint();
    }
};
class WaveformScope : public juce::Component
{
public:
    WaveformScope(RS40AudioProcessor& p) : proc(p)
    {
        setInterceptsMouseClicks(false, false);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xFF1E1A17));
        g.fillRoundedRectangle(bounds, 3.0f);
        g.setColour(juce::Colour(0xFF5E554A));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
        g.setColour(juce::Colour(0xFF5E554A).withAlpha(0.4f));
        g.drawHorizontalLine((int)(bounds.getCentreY()), bounds.getX() + 2.0f, bounds.getRight() - 2.0f);
        g.setColour(juce::Colour(0xFFD4912A).withAlpha(0.06f));
        g.drawHorizontalLine((int)(bounds.getY() + bounds.getHeight() * 0.25f), bounds.getX() + 2.0f, bounds.getRight() - 2.0f);
        g.drawHorizontalLine((int)(bounds.getY() + bounds.getHeight() * 0.75f), bounds.getX() + 2.0f, bounds.getRight() - 2.0f);
        const auto& buffer = proc.getScopeBuffer();
        int writeIdx = proc.getScopeWriteIndex();
        int bufSize = (int)buffer.size();

        auto insetBounds = bounds.reduced(4.0f, 4.0f);
        float w = insetBounds.getWidth();
        float h = insetBounds.getHeight();
        float centreY = insetBounds.getCentreY();

        auto amber = juce::Colour(0xFFD4912A);
        auto crimson = juce::Colour(0xFF8B1A2B);
        for (int i = 0; i < bufSize - 1; ++i)
        {
            int idx0 = (writeIdx + i) % bufSize;
            int idx1 = (writeIdx + i + 1) % bufSize;
            float s0 = juce::jlimit(-1.0f, 1.0f, buffer[(size_t)idx0]);
            float s1 = juce::jlimit(-1.0f, 1.0f, buffer[(size_t)idx1]);

            float x0 = insetBounds.getX() + (float)i / (float)(bufSize - 1) * w;
            float x1 = insetBounds.getX() + (float)(i + 1) / (float)(bufSize - 1) * w;
            float y0 = centreY - s0 * (h * 0.45f);
            float y1 = centreY - s1 * (h * 0.45f);
            float amp = std::max(std::abs(s0), std::abs(s1));
            float blend = juce::jlimit(0.0f, 1.0f, (amp - 0.3f) / 0.7f);
            auto segColour = amber.interpolatedWith(crimson, blend);

            g.setColour(segColour);
            g.drawLine(x0, y0, x1, y1, 1.5f);
        }
    }

private:
    RS40AudioProcessor& proc;
};

class RS40AudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    RS40AudioProcessorEditor(RS40AudioProcessor& p)
        : AudioProcessorEditor(&p), processor(p)
    {
        brandFont = juce::Font(juce::Typeface::createSystemTypefaceFor(
            BinaryData::IBMPlexMonoBold_ttf, BinaryData::IBMPlexMonoBold_ttfSize));
        rs40LookAndFeel.setBrandFont(brandFont);
        setLookAndFeel(&rs40LookAndFeel);
        setSize(780, 512);
        bgColor        = juce::Colour(0xFF0A0808);
        panelColor     = juce::Colour(0xFF1E1A17);
        accentColor    = juce::Colour(0xFFD4912A);
        lightAccent    = juce::Colour(0xFFE8B44C);
        textColor      = juce::Colour(0xFFE8DCC8);
        dimColor       = juce::Colour(0xFF9A8E7E);
        mutedColor     = juce::Colour(0xFF5E554A);
        highlightColor = juce::Colour(0xFF8B1A2B);
        setupSlider(gainSlider, gainAttachment, "gain", "GAIN");
        setupSlider(loGainSlider, loGainAttachment, "loGain", "LO +/-");
        setupSlider(loTuneSlider, loTuneAttachment, "loTune", "LO FREQ");
        setupSlider(midGainSlider, midGainAttachment, "midGain", "MID +/-");
        setupSlider(midTuneSlider, midTuneAttachment, "midTune", "MID FREQ");
        setupSlider(hiGainSlider, hiGainAttachment, "hiGain", "HI +/-");
        setupSlider(hiTuneSlider, hiTuneAttachment, "hiTune", "HI FREQ");
        setupSlider(outputSlider, outputAttachment, "outputLevel", "OUTPUT");
        setupSlider(distBlendSlider, distBlendAttachment, "distBlend", "BLEND");
        setupSlider(distDriveSlider, distDriveAttachment, "distDrive", "DRIVE");

        distButton.setButtonText("DIST");
        addAndMakeVisible(distButton);
        distButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processor.getAPVTS(), "distEnabled", distButton);
        setupSlider(analogSlider, analogAttachment, "analogCharacter", "ANALOG");
        addAndMakeVisible(peakLED);
        addAndMakeVisible(diodeSchematic);
        addAndMakeVisible(scopeDisplay);
        startTimerHz(30);
    }

    ~RS40AudioProcessorEditor() override
    {
        setLookAndFeel(nullptr);
        stopTimer();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(bgColor);
        {
            float cx = (float)getWidth() * 0.5f;
            float cy = -50.0f;
            juce::ColourGradient bloom(
                juce::Colour(0xFF8B1A2B).withAlpha(0.10f), cx, cy,
                juce::Colours::transparentBlack, cx, cy + 350.0f, true);
            g.setGradientFill(bloom);
            g.fillRect(getLocalBounds().toFloat());
        }
        {
            float cx = (float)getWidth() * 0.5f;
            float cy = (float)getHeight() + 50.0f;
            juce::ColourGradient bloom(
                juce::Colour(0xFF8B1A2B).withAlpha(0.10f), cx, cy,
                juce::Colours::transparentBlack, cx - 350.0f, cy - 350.0f, true);
            g.setGradientFill(bloom);
            g.fillRect(getLocalBounds().toFloat());
        }
        {
            auto boundsF = getLocalBounds().toFloat();
            float w = (float)getWidth();
            float h = (float)getHeight();
            float r = 300.0f;
            float alpha = 0.08f;
            auto amberGlow = juce::Colour(0xFFD4912A).withAlpha(alpha);

            struct Corner { float cx, cy, dx, dy; };
            Corner corners[] = {
                { -50.0f,   -50.0f,   r,  r },
                { w + 50.0f, -50.0f,  -r,  r },
                { -50.0f,   h + 50.0f, r, -r },
                { w + 50.0f, h + 50.0f, -r, -r }
            };

            for (auto& c : corners)
            {
                juce::ColourGradient bloom(
                    amberGlow, c.cx, c.cy,
                    juce::Colours::transparentBlack, c.cx + c.dx, c.cy + c.dy, true);
                g.setGradientFill(bloom);
                g.fillRect(boundsF);
            }
        }
        g.setColour(accentColor.withAlpha(0.06f));
        for (int x = 68; x < getWidth(); x += 68)
            g.drawVerticalLine(x, 0.0f, (float)getHeight());
        for (int y = 68; y < getHeight(); y += 68)
            g.drawHorizontalLine(y, 0.0f, (float)getWidth());
        {
            auto brandX = 20;
            auto brandY = 12;
            auto boxSize = 24;
            g.setColour(accentColor);
            g.drawRect(brandX, brandY, boxSize, boxSize, 2);
            g.setColour(accentColor.withAlpha(0.3f));
            g.drawRect(brandX + 3, brandY + 3, boxSize - 6, boxSize - 6, 1);
            g.setColour(accentColor);
            g.setFont(brandFont.withHeight(10.0f));
            g.drawText("RS", brandX, brandY, boxSize, boxSize, juce::Justification::centred);
            auto textX = brandX + boxSize + 8;
            g.setColour(textColor);
            g.setFont(brandFont.withHeight(10.0f));
            g.drawText("RANDOM", textX, brandY, 100, boxSize / 2, juce::Justification::centredLeft);
            g.setColour(accentColor);
            g.drawText("SAMPLES", textX, brandY + boxSize / 2, 100, boxSize / 2, juce::Justification::centredLeft);
        }
        g.setColour(accentColor);
        g.setFont(brandFont.withHeight(28.0f));
        g.drawText("RS-40", getLocalBounds().removeFromTop(50),
                   juce::Justification::centred);
        auto drawDivider = [&](float yPos) {
            float x1 = 20.0f, x2 = (float)(getWidth() - 20);
            juce::ColourGradient grad(
                juce::Colour(0xFF8B1A2B), x1, yPos,
                juce::Colour(0xFF5C1018), x2, yPos, false);
            grad.addColour(0.5, juce::Colour(0xFFD4912A).withAlpha(0.3f));
            g.setGradientFill(grad);
            g.fillRect(x1, yPos, x2 - x1, 1.0f);
        };
        drawDivider(45.0f);
        drawDivider(200.0f);
        drawDivider(355.0f);
        g.setColour(dimColor);
        g.setFont(brandFont.withHeight(10.0f));
        g.drawText("PREAMP", 20, 48, 100, 15, juce::Justification::left);
        g.drawText("EFFECTS LOOP - 1N914 DIODE CLIPPER", 20, 203, 300, 15, juce::Justification::left);
        g.drawText("CHARACTER", 20, 358, 100, 15, juce::Justification::left);
        g.setColour(mutedColor);
        g.setFont(brandFont.withHeight(9.0f));
        g.drawText("v1.0.0", getWidth() - 70, getHeight() - 22, 55, 14, juce::Justification::right);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        area.removeFromTop(83);
        auto eqRow = area.removeFromTop(110);
        int sliderWidth = 85;
        int startX = 20;
        int y = eqRow.getY() + 5;
        int h = 100;

        gainSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;

        loGainSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;
        loTuneSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;

        midGainSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;
        midTuneSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;

        hiGainSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;
        hiTuneSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;

        outputSlider.setBounds(startX, y, sliderWidth, h);
        area.removeFromTop(25);
        auto distRow = area.removeFromTop(120);
        startX = 20;
        y = distRow.getY() + 25;
        h = 100;

        distButton.setBounds(startX, y + 30, 70, 35);
        startX += 80;
        distDriveSlider.setBounds(startX, y, sliderWidth, h);
        startX += sliderWidth + 5;
        distBlendSlider.setBounds(startX, y, sliderWidth, h);
        int ledSize = 44;
        peakLED.setBounds(getWidth() - 52 - ledSize / 2, 200 + (355 - 200 - ledSize) / 2, ledSize, ledSize);
        int schematicX = startX + sliderWidth + 20;
        int schematicW = getWidth() - 80 - schematicX;
        int schematicH = 110;
        int schematicY = 200 + (355 - 200 - schematicH) / 2;
        diodeSchematic.setBounds(schematicX, schematicY, schematicW, schematicH);
        area.removeFromTop(35);
        auto charRow = area.removeFromTop(120);
        y = charRow.getY() + 25;
        analogSlider.setBounds(20, y, sliderWidth, 100);
        int scopeHeight = 80;
        int scopeY = 355 + (getHeight() - 355 - scopeHeight) / 2;
        scopeDisplay.setBounds(130, scopeY, getWidth() - 150, scopeHeight);
    }

private:
    RS40AudioProcessor& processor;
    RS40LookAndFeel rs40LookAndFeel;
    juce::OwnedArray<juce::Label> ownedLabels;
    juce::Font brandFont;
    juce::Colour bgColor, panelColor, accentColor, lightAccent;
    juce::Colour textColor, dimColor, mutedColor, highlightColor;
    juce::Slider gainSlider, loGainSlider, loTuneSlider;
    juce::Slider midGainSlider, midTuneSlider;
    juce::Slider hiGainSlider, hiTuneSlider;
    juce::Slider outputSlider;
    juce::Slider distBlendSlider, distDriveSlider;
    juce::ToggleButton distButton;
    juce::Slider analogSlider;
    PeakLED peakLED;
    DiodeSchematic diodeSchematic;
    WaveformScope scopeDisplay { processor };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gainAttachment, loGainAttachment, loTuneAttachment,
        midGainAttachment, midTuneAttachment,
        hiGainAttachment, hiTuneAttachment,
        outputAttachment, distBlendAttachment, distDriveAttachment,
        analogAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        distButtonAttachment;

    void setupSlider(juce::Slider& slider,
                     std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                     const juce::String& paramId,
                     const juce::String& label)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        slider.setColour(juce::Slider::rotarySliderFillColourId, accentColor);
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, mutedColor);
        slider.setColour(juce::Slider::textBoxTextColourId, accentColor);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        slider.setName(label);
        auto* labelComp = ownedLabels.add(new juce::Label({}, label));
        labelComp->setFont(brandFont.withHeight(11.0f));
        labelComp->setColour(juce::Label::textColourId, textColor.withAlpha(0.7f));
        labelComp->setJustificationType(juce::Justification::centred);
        labelComp->attachToComponent(&slider, false);

        addAndMakeVisible(slider);
        addAndMakeVisible(labelComp);

        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.getAPVTS(), paramId, slider);
        auto* param = processor.getAPVTS().getParameter(paramId);
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
            slider.setDoubleClickReturnValue(true, ranged->convertFrom0to1(ranged->getDefaultValue()));
    }

    void timerCallback() override
    {
        double peak = processor.getPeakLevel();
        peakLED.setActive(peak > 0.9);
        processor.resetPeakLevel();
        scopeDisplay.repaint();
        diodeSchematic.setDistEnabled(*processor.getAPVTS().getRawParameterValue("distEnabled") > 0.5f);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RS40AudioProcessorEditor)
};
