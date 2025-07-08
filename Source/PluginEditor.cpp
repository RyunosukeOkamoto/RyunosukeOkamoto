/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MetalDistortionAudioProcessorEditor::MetalDistortionAudioProcessorEditor (MetalDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(metalSlider);
    addAndMakeVisible(metalButton);
    addAndMakeVisible(bassSlider);
    addAndMakeVisible(bassButton);
    addAndMakeVisible(smoothSlider);
    addAndMakeVisible(smoothButton);
    
    backgroundImage = juce::ImageFileFormat::loadFrom(BinaryData::back_png, BinaryData::back_pngSize);
    birdImage       = juce::ImageFileFormat::loadFrom(BinaryData::bird_png, BinaryData::bird_pngSize);
    knobImage       = juce::ImageFileFormat::loadFrom(BinaryData::nob_png,  BinaryData::nob_pngSize);
    onImage         = juce::ImageFileFormat::loadFrom(BinaryData::on_png,   BinaryData::on_pngSize);
    offImage        = juce::ImageFileFormat::loadFrom(BinaryData::off_png,  BinaryData::off_pngSize);


    customKnobLookAndFeel = std::make_unique<CustomKnobLookAndFeel>(knobImage);
    customToggleLookAndFeel = std::make_unique<CustomToggleLookAndFeel>(onImage, offImage);

    // それぞれのスライダー／ボタンにLookAndFeelを適用
    metalSlider.setLookAndFeel(customKnobLookAndFeel.get());
    bassSlider.setLookAndFeel(customKnobLookAndFeel.get());
    smoothSlider.setLookAndFeel(customKnobLookAndFeel.get());

    metalButton.setLookAndFeel(customToggleLookAndFeel.get());
    bassButton.setLookAndFeel(customToggleLookAndFeel.get());
    smoothButton.setLookAndFeel(customToggleLookAndFeel.get());
    
    
    metalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "metalAmount", metalSlider);
    metalButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "metalOn", metalButton);

    
    bassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "bassAmount", bassSlider);
    bassButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "bassOn", bassButton);

    smoothAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "smoothAmount", smoothSlider);
    smoothButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "smoothOn", smoothButton);
    
    

    metalSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    bassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    smoothSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    
    metalSlider.setRange(0.1, 10.0);
    bassSlider.setRange(0.1, 10.0);
    smoothSlider.setRange(0.1, 1.0);
    
    float rotaryStart = juce::degreesToRadians(45.0f);
    float rotaryEnd   = juce::degreesToRadians(315.0f);

    metalSlider.setRotaryParameters(rotaryStart, rotaryEnd,true);
    bassSlider.setRotaryParameters(rotaryStart, rotaryEnd, true);
    smoothSlider.setRotaryParameters(rotaryStart, rotaryEnd, true);
    
    
    metalSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    smoothSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

    
    setSize(360, 220);
}



MetalDistortionAudioProcessorEditor::~MetalDistortionAudioProcessorEditor()
{
}

//==============================================================================
void MetalDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
   
    g.drawImage(backgroundImage, getLocalBounds().toFloat());

   
    int birdW = birdImage.getWidth();
        int birdH = birdImage.getHeight();
        int birdX = getWidth() - birdW + 5;
        int birdY = -25;

        g.drawImage(birdImage, birdX, birdY, birdW, birdH, 0, 0, birdW, birdH);
    
    
    
    int knobW = 100, knobH = 100, margin = 20, gap = 20, y0 = 60;
       int labelH = 20 ;

       g.setColour(juce::Colours::white);
       g.setFont(juce::Font(16.0f, juce::Font::bold));

       // Metalノブ
       g.drawFittedText("Metal", margin, y0 - labelH, knobW, labelH, juce::Justification::centred, 1);

       // Bassノブ
       g.drawFittedText("Bass", margin + knobW + gap, y0 - labelH, knobW, labelH, juce::Justification::centred, 1);

       // Smoothノブ
       g.drawFittedText("Smooth", margin + (knobW + gap) * 2, y0 - labelH, knobW, labelH, juce::Justification::centred, 1);

}

void MetalDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int knobW = 100, knobH = 100, btnH = 30;
    int margin = 20;
    int gap = 20;
    int y0 = 60;
    int btnY = y0 + knobH + 10;

    int setbutton =30;
    
    metalSlider.setBounds(margin, y0, knobW, knobH);
    metalButton.setBounds(margin + setbutton, btnY, knobW, btnH);

    bassSlider.setBounds(margin + knobW + gap, y0, knobW, knobH);
    bassButton.setBounds(margin + knobW + gap + setbutton, btnY, knobW, btnH);

    smoothSlider.setBounds(margin + (knobW + gap) * 2, y0, knobW, knobH);
    smoothButton.setBounds(margin + (knobW + gap) * 2 + setbutton, btnY, knobW, btnH);

}
