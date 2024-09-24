/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomSlider : juce::Slider
{
    CustomSlider() : juce::Slider(juce::Slider::SliderStyle::LinearBar,
                                        juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {
        
    }
};

//==============================================================================
/**
*/
class FirstDistoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FirstDistoAudioProcessorEditor (FirstDistoAudioProcessor&);
    ~FirstDistoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FirstDistoAudioProcessor& audioProcessor;
    
    CustomSlider preDelaySlider,
    dryWetSlider;
    
    juce::Label preDelayLabel;
    juce::Label dryWetLabel;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment preDelaySliderAttachment, dryWetSliderAttachment;
        
    
    std::vector<juce::Component*> getComps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirstDistoAudioProcessorEditor)
};
