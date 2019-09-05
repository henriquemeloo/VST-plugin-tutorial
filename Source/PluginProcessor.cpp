/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Tutorial1AudioProcessor::Tutorial1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	addParameter(gain = new AudioParameterFloat("gain", "Gain (dB)", -100.0f, 12.0f, 0.0f));
	addParameter(pan = new AudioParameterFloat("pan", "Pan", -1.0f, 1.0f, 0.0f));
	addParameter(toggleAutoPan = new AudioParameterBool("toggleAutoPan", "Toggle Auto Pan", false));
	addParameter(autoPan = new AudioParameterFloat("autoPan", "Auto Pan Freq (BPM)", 1.0f, 300.0f, 120.0f));
}

Tutorial1AudioProcessor::~Tutorial1AudioProcessor()
{
}

//==============================================================================
const String Tutorial1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Tutorial1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Tutorial1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Tutorial1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Tutorial1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Tutorial1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Tutorial1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Tutorial1AudioProcessor::setCurrentProgram (int index)
{
}

const String Tutorial1AudioProcessor::getProgramName (int index)
{
    return {};
}

void Tutorial1AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Tutorial1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Tutorial1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Tutorial1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void Tutorial1AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    auto* channelDataL = buffer.getWritePointer(0);
	auto* channelDataR = buffer.getWritePointer(1);

	float gSlider = pow(10, (gain->get() / 20));
	float panAngle = (pan->get() + 1) * 45;
	float panLeft = cos(double_Pi * panAngle / 180);
	float panRight = sin(double_Pi * panAngle / 180);

	float autoPanAngle, autoPanLeft, autoPanRight;
	float autoPanFreq = autoPan->get() / 60;
	float samplingPeriod = 1.0 / getSampleRate();
	int maxN = 1 / (autoPanFreq * samplingPeriod);

    // ..do something to the data...
	for (int i = 0; i < buffer.getNumSamples(); i++) {
		if (toggleAutoPan->get()) {
			// autopanning
			/*
			Calculate constant power panning with the angle of panning
			varying sinusoidally in given frequency.
			*/
			autoPanAngle = (double_Pi / 180) * (
				sin(2 * double_Pi * autoPanFreq * n * samplingPeriod) + 1
				) * 45;
			autoPanLeft = cos(autoPanAngle);
			autoPanRight = sin(autoPanAngle);

			n++;
		}
		else {
			autoPanLeft = 1;
			autoPanRight = 1;
		}
		channelDataL[i] *= gSlider * panLeft * autoPanLeft;
		channelDataR[i] *= gSlider * panRight * autoPanRight;

		if (n > maxN) {
			n = 0;
		}
	}
}

//==============================================================================
bool Tutorial1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Tutorial1AudioProcessor::createEditor()
{
    //return new Tutorial1AudioProcessorEditor (*this);
	return new GenericAudioProcessorEditor(this);
}

//==============================================================================
void Tutorial1AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Tutorial1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Tutorial1AudioProcessor();
}
