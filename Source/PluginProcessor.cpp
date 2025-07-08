/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
MetalDistortionAudioProcessor::MetalDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                    ),
    parameters(*this, nullptr, "PARAMETERS",
      {
            std::make_unique<juce::AudioParameterFloat>("metalAmount", "Metal Amount", 0.1f, 10.0f, 2.0f),
            std::make_unique<juce::AudioParameterBool>("metalOn", "Metal On", true),

            std::make_unique<juce::AudioParameterFloat>("bassAmount", "Bass Amount", 0.1f, 10.0f, 2.0f),
            std::make_unique<juce::AudioParameterBool>("bassOn", "Bass On", false),

            std::make_unique<juce::AudioParameterFloat>("smoothAmount", "Smooth Amount", 0.1f, 1.0f, 0.5f),
            std::make_unique<juce::AudioParameterBool>("smoothOn", "Smooth On", false)
      })
#endif
{
}

MetalDistortionAudioProcessor::~MetalDistortionAudioProcessor()
{
}

//==============================================================================
const juce::String MetalDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MetalDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MetalDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MetalDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MetalDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MetalDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MetalDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MetalDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MetalDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void MetalDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MetalDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MetalDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MetalDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MetalDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    float metalAmt   = parameters.getRawParameterValue("metalAmount")->load();
    bool  metalOn    = parameters.getRawParameterValue("metalOn")->load() > 0.5f;
    float bassAmt    = parameters.getRawParameterValue("bassAmount")->load();
    bool  bassOn     = parameters.getRawParameterValue("bassOn")->load() > 0.5f;
    float smoothAmt  = parameters.getRawParameterValue("smoothAmount")->load();
    bool  smoothOn   = parameters.getRawParameterValue("smoothOn")->load() > 0.5f;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        static float subOsc = 0.0f;
        float prevLow = 0.0f;
        
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float x = samples[sample];

            
            if (metalOn)
                x = std::tanh(x * metalAmt) + 0.3f * std::sin(x * 10.0f * metalAmt);

            
            if (bassOn) {

                prevLow = 0.98f * prevLow + 0.02f * x;
                       
                       subOsc = 0.95f * subOsc + 0.05f * (prevLow > 0 ? 1.0f : -1.0f);
                      
                       float lowDist = std::tanh((prevLow + subOsc * 0.3f) * bassAmt);
                       x += lowDist * 0.3f;
            }


            if (smoothOn){
                
                float drive = 1.2f * smoothAmt;
                float clipped = std::tanh(x * drive);
                float asym = x - 3.0f * std::pow(x, 3);
                float warmth = 0.6f * clipped + 0.4f * asym;
                float y = 0.5f * warmth + 1.0f * x;
                x = y;
            }
                
            samples[sample] = juce::jlimit(-1.0f, 1.0f, x);
        }
    }
}

//==============================================================================
bool MetalDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MetalDistortionAudioProcessor::createEditor()
{
    return new MetalDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void MetalDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MetalDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MetalDistortionAudioProcessor();
}
