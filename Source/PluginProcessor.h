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

//caused error build: Command PhaseScriptExecution failed with a nonzero exit code
//so i commented it out for now
//juce::dsp::ConvolutionMessageQueue convQueue;



ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using MyConvolution = juce::dsp::Convolution;
using MixControl = juce::dsp::DryWetMixer<float>;
using MyDelay = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;

enum ChainPositions
{
    DryWet,
    PreDelayTime,
};

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
    
    MyConvolution stereoConv{juce::dsp::Convolution::Latency{128}};
    MixControl mixerLeft, mixerRight;
//    StereoDelay dChain;
    
    MyDelay preDelay{9600000};
    
    double currSampleRate{0.0};
    

    
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirstDistoAudioProcessor)
};
