/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
    float dryWet { 0 };
    float preDelayTime { 10 };
};

juce::dsp::ConvolutionMessageQueue queue;



ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using Convolution = juce::dsp::Convolution;
using MixControl = juce::dsp::DryWetMixer<float>;
using Delay = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;
//using StereoDelay = juce::dsp::ProcessorDuplicator<juce::dsp::DelayLine<float>>;
//using ReverbChain = juce::dsp::ProcessorChain<StereoDelay>;
//==============================================================================
/**
*/
class FirstDistoAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    FirstDistoAudioProcessor();
    ~FirstDistoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
private:
    
    Convolution stereoConv{juce::dsp::Convolution::Latency{128}, queue};
    MixControl mixerLeft, mixerRight;
//    StereoDelay dChain;
    
    Delay preDelay{9600000};
    
    double currentSampleRate{0.0};
    
    enum ChainPositions
    {
        DryWet,
        PreDelayTime,
    };
    
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirstDistoAudioProcessor)
};
