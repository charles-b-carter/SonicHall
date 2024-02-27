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
//    juce::dsp::ProcessSpec stereoSpec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;
    
//    stereoSpec.maximumBlockSize = samplesPerBlock;
//    stereoSpec.numChannels = 2;
//    stereoSpec.sampleRate = sampleRate;
    

//    leftChain.prepare(spec);
//    rightChain.prepare(spec);
    leftConv.prepare(spec);
//    rightConv.prepare(spec);
    
    mixerLeft.prepare(spec);
    mixerRight.prepare(spec);

//    juce::File pathLeft("/Users/charliecarter/Desktop/JUCE Projects/FirstDisto/Source/Resources/qv_room_l.wav");
//    juce::File pathRight("/Users/charliecarter/Desktop/JUCE Projects/FirstDisto/Source/Resources/qv_room_r.wav");
      juce::File path("/Users/charliecarter/Desktop/JUCE Projects/FirstDisto/Source/Resources/qv_room_stereo.wav");
    

    if(/*pathLeft.exists()*/path.exists()){
        
//        auto& convolutionLeft = leftChain.template get<0>();
//        auto& convolutionRight = rightChain.template get<0>();
//        auto& convolutionStereo = leftChain.template get<0>();
//         auto& convolutionLeft = leftConv;
//         auto& convolutionRight = rightConv;


        leftConv.loadImpulseResponse(path, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0);
//        leftConv.loadImpulseResponse(pathLeft, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0);
//        rightConv.loadImpulseResponse(pathRight, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0);

    }
    
    auto chainSettings = getChainSettings(apvts);
    
    auto mixerLeft = MixControl();
    auto mixerRight = MixControl();
    
    
    mixerLeft.setWetMixProportion(chainSettings.dryWet);
    mixerRight.setWetMixProportion(chainSettings.dryWet);
//
//    leftChain.reset();
//    rightChain.reset();

    mixerLeft.reset();
    mixerRight.reset();
        
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    auto chainSettings = getChainSettings(apvts);
    
    mixerLeft.setWetMixProportion(chainSettings.dryWet);
    mixerRight.setWetMixProportion(chainSettings.dryWet);
    
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    juce::dsp::ProcessContextReplacing<float> context(block);
//
    mixerLeft.pushDrySamples(leftContext.getInputBlock());
    mixerRight.pushDrySamples(rightContext.getInputBlock());
    
//    leftChain.process(leftContext);
//    rightChain.process(rightContext);
//    leftChain.process(context);
    
    leftConv.process(context);
//    rightConv.process(rightContext);
    
    mixerLeft.mixWetSamples(leftContext.getOutputBlock());
    mixerRight.mixWetSamples(rightContext.getOutputBlock());


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
