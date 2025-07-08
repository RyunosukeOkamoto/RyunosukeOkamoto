/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/


class CustomKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomKnobLookAndFeel(const juce::Image& knobImg) : knobImage(knobImg) {}

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override
    {
        if (! knobImage.isValid()) return;

        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        const float cx = x + width * 0.5f;
        const float cy = y + height * 0.5f;

        juce::AffineTransform transform =
        juce::AffineTransform::rotation(angle, knobImage.getWidth() * 0.5f, knobImage.getHeight() * 0.5f)
            .translated(cx - knobImage.getWidth() * 0.5f, cy - knobImage.getHeight() * 0.5f);

        g.drawImageTransformed(knobImage, transform, false);
    }

private:
    juce::Image knobImage;
};


class CustomToggleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomToggleLookAndFeel(const juce::Image& onImg, const juce::Image& offImg)
        : onImage(onImg), offImage(offImg) {}

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool, bool) override
    {
        const auto& img = button.getToggleState() ? onImage : offImage;
        g.drawImageAt(img, 0, 0);
    }
private:
    juce::Image onImage, offImage;
};


class MetalDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MetalDistortionAudioProcessorEditor (MetalDistortionAudioProcessor&);
    ~MetalDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::Slider metalSlider, bassSlider, smoothSlider;
    juce::ToggleButton metalButton, bassButton, smoothButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> metalAttachment, bassAttachment, smoothAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> metalButtonAttachment, bassButtonAttachment, smoothButtonAttachment;

    juce::Image backgroundImage, birdImage, knobImage, onImage, offImage;

    std::unique_ptr<CustomKnobLookAndFeel> customKnobLookAndFeel;
    std::unique_ptr<CustomToggleLookAndFeel> customToggleLookAndFeel;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MetalDistortionAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetalDistortionAudioProcessorEditor)
};

