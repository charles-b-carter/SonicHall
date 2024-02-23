/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FirstDistoAudioProcessor::FirstDistoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FirstDistoAudioProcessor::~FirstDistoAudioProcessor()
{
}

//==============================================================================
const juce::String FirstDistoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FirstDistoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FirstDistoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FirstDistoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FirstDistoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FirstDistoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FirstDistoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FirstDistoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FirstDistoAudioProcessor::getProgramName (int index)
{
    return {};
}

void FirstDistoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FirstDistoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    

    leftChain.prepare(spec);
    rightChain.prepare(spec);
    mixerLeft.prepare(spec);
    mixerRight.prepare(spec);

    juce::File path("/Users/charliecarter/Desktop/JUCE Projects/FirstDisto/Source/Resources/cassette_recorder_mono.wav");
    if(path.exists()){
        
        auto& convolutionLeft = leftChain.template get<0>();
        auto& convolutionRight = rightChain.template get<0>();

        
        
        convolutionLeft.loadImpulseResponse(path, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 1024);
        convolutionRight.loadImpulseResponse(path, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 1024);

    }
    
    auto chainSettings = getChainSettings(apvts);
    
//    auto mixerLeft = MixControl();
//    auto mixerRight = MixControl();
    
    
    mixerLeft.setWetMixProportion(chainSettings.dryWet);
    mixerRight.setWetMixProportion(chainSettings.dryWet);

    
    leftChain.reset();
    rightChain.reset();
    
//    mixerLeft.reset();
//    mixerRight.reset();
    

    
}

void FirstDistoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FirstDistoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FirstDistoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    auto chainSettings = getChainSettings(apvts);
    
//    auto mixerLeft = MixControl();
//    auto mixerRight = MixControl();
    
    mixerLeft.setWetMixProportion(chainSettings.dryWet);
    mixerRight.setWetMixProportion(chainSettings.dryWet);
    
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
//    juce::dsp::ProcessContextReplacing<float> context(block);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

//    auto& leftInBlock = leftContext.getInputBlock();
//    auto& leftOutBlock = leftContext.getOutputBlock();
//
//    auto& rightInBlock = rightContext.getInputBlock();
//    auto& rightOutBlock = rightContext.getOutputBlock();
    
    mixerLeft.pushDrySamples(leftContext.getInputBlock());
    mixerRight.pushDrySamples(rightContext.getInputBlock());
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
    
    mixerLeft.mixWetSamples(leftContext.getOutputBlock());
    mixerRight.mixWetSamples(rightContext.getOutputBlock());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//    }
}

//==============================================================================
bool FirstDistoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FirstDistoAudioProcessor::createEditor()
{
//    return new FirstDistoAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void FirstDistoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FirstDistoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    
   settings.dryWet = apvts.getRawParameterValue("Dry/Wet")->load();
    
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout
    FirstDistoAudioProcessor::createParameterLayout()
{
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Dry/Wet", 1),"Dry/Wet",juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), 0.f));
        
        return layout;
    }
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FirstDistoAudioProcessor();
}
