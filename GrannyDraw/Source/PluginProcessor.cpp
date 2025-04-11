/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
GrannyDrawAudioProcessor::GrannyDrawAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

GrannyDrawAudioProcessor::~GrannyDrawAudioProcessor()
{
}


//AudioProcessorValueTreeState::ParameterLayout GrannyDrawAudioProcessor::createParameterLayout(){
//    std::vector<std::unique_ptr<RangedAudioParameter>> params;
//    
//    params.push_back (std::make_unique<AudioParameterFloat>("PITCH","Pitch",-12.f,12.f,0.f));
//    
//    return {params.begin() , params.end()};
//}
//==============================================================================
const String GrannyDrawAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GrannyDrawAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GrannyDrawAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GrannyDrawAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GrannyDrawAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GrannyDrawAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GrannyDrawAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GrannyDrawAudioProcessor::setCurrentProgram (int index)
{
}

const String GrannyDrawAudioProcessor::getProgramName (int index)
{
    return {};
}

void GrannyDrawAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GrannyDrawAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    pitchShiftEffect.setFs(sampleRate);
    interval = 60.0 / bpm * sampleRate;
}
void getNextAduioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto buffer = bufferToFill.numSamples;
}

void GrannyDrawAudioProcessor::countSamples (int bufferSize)
{
    totalSamples += bufferSize;
}

void GrannyDrawAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GrannyDrawAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GrannyDrawAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    const double sampleRate = getSampleRate(); // Get sample rate for accurate time-based calculations

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (pitchCurve.empty())
        return;
    
//    playHead = this->getPlayHead();
//    if (playHead != nullptr)
//    {
//    playHead->getPosition();
//    }
//    auto bpm = cpi.bpm;
//
////    const double bpm = 120.0;
//    const double timeSigNumerator = 4.0; // 4/4 time
//    const double secondsPerBeat = 60.0 / bpm;
    auto bpm = cpi.bpm;
    auto timeSigNum = cpi.timeSigNumerator;
//    auto timeSigDen = cpi.timeSigDenominator;
    auto timeInSamples = cpi.timeInSamples;
    const double samplesPerBar = (60 / bpm) * sampleRate * timeSigNum;

    // Track sample count over time
//    static double sampleCounter = 0;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Calculate phase of the current bar (0.0 to 1.0)
            double phase = fmod(timeInSamples, samplesPerBar) / samplesPerBar;
            int curveIndex = juce::jlimit(0, static_cast<int>(pitchCurve.size()) - 1,
                                          static_cast<int>(phase * pitchCurve.size()));
            float pitch = pitchCurve[curveIndex];
            pitchShiftEffect.setPitch(pitch);

            float inputSample = channelData[sample];
            float outputSample = pitchShiftEffect.processSample(inputSample, channel);
            channelData[sample] = outputSample;

//            sampleCounter += 1.0;
        }
    }
}





//==============================================================================
bool GrannyDrawAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GrannyDrawAudioProcessor::createEditor()
{
    return new GrannyDrawAudioProcessorEditor (*this);
}

//==============================================================================
void GrannyDrawAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
//    auto currentState = state.copyState();
//    std::unique_ptr<XmlElement> xml(currentState.createXml());
//    copyXmlToBinary(*xml, destData);
}

void GrannyDrawAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    //MemoryBlock data_block (data, (size_t) sizeInBytes);
//    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
//    if (xmlState && xmlState->hasTagName(state.state.getType())){
//        state.replaceState(ValueTree::fromXml(*xmlState));
    }

void GrannyDrawAudioProcessor::setPitchCurve(const std::vector<float>& newCurve)
{
    pitchCurve = newCurve;
}
std::vector<float> GrannyDrawAudioProcessor::getPitchCurve() const
{
    return pitchCurve;
}
void GrannyDrawAudioProcessor::setPitchPlayheadIndex(int index)
{
    pitchPlayhead = index;
}

int GrannyDrawAudioProcessor::getPitchPlayheadIndex() const
{
    return pitchPlayhead;
}

size_t GrannyDrawAudioProcessor::getPitchCurveLength() const
{
    return pitchCurve.size();
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrannyDrawAudioProcessor();
}
