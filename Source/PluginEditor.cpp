/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FirstDistoAudioProcessorEditor::FirstDistoAudioProcessorEditor (FirstDistoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
preDelaySliderAttachment(audioProcessor.apvts, "Pre-Delay", preDelaySlider),
dryWetSliderAttachment(audioProcessor.apvts, "Dry/Wet", dryWetSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for( auto* comp : getComps()){
        addAndMakeVisible(comp);
    }
    preDelayLabel.setText("Pre-Delay Amount", juce::dontSendNotification);
    preDelayLabel.attachToComponent(&preDelaySlider, false);
    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.attachToComponent(&dryWetSlider, false);
    
    setSize (400, 200);
}

FirstDistoAudioProcessorEditor::~FirstDistoAudioProcessorEditor()
{
}

//==============================================================================
void FirstDistoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::transparentWhite);
    g.setFont (15.0f);
//    g.drawFittedText ("Quadraverb Room Reverb", getLocalBounds(), juce::Justification::centredTop, 1);
    
    
}

void FirstDistoAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.3);
    auto wetDryArea = bounds.removeFromTop(bounds.getHeight() * 0.2);
    auto spaceArea = bounds.removeFromTop(bounds.getHeight() * 0.5);
    auto preDelayArea = bounds.removeFromTop(bounds.getHeight() * 0.5);
    
    
    preDelaySlider.setBounds(preDelayArea);
    dryWetSlider.setBounds(wetDryArea);
    
}

std::vector<juce::Component*> FirstDistoAudioProcessorEditor::getComps(){
    return{
        &preDelaySlider,
        &dryWetSlider,
        &preDelayLabel,
        &dryWetLabel
    };
}
