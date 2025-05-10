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
                       ),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}


GrannyDrawAudioProcessor::~GrannyDrawAudioProcessor()
{
}


AudioProcessorValueTreeState::ParameterLayout GrannyDrawAudioProcessor::createParameterLayout(){
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back (std::make_unique<AudioParameterFloat>("snap","Snap",0.f,100.f,0.f));
    
    params.push_back (std::make_unique<AudioParameterChoice>("loopRate", "Loop Rate",
                                                             juce::StringArray({ "0.25x", "0.5x", "1x", "2x", "4x", "8x", "16x" }),
                                                             2));
    
    return {params.begin() , params.end()};
}
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
    smoothedPitch.reset(sampleRate, 0.02); // 20ms smoothing
    previousMuteStates.resize(getTotalNumInputChannels(), false);
    muteGains.resize(getTotalNumInputChannels(), 1.0f);

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

float quantizePitch(float pitch, float snapAmount)
{
    float t = juce::jlimit(0.0f, 1.0f, snapAmount / 100.0f);
    float snapped = std::round(pitch);
    return juce::jmap(t, pitch, snapped);
}





void GrannyDrawAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (pitchCurve.empty())
        return;

    playHead = getPlayHead();
    if (playHead != nullptr)
    {
        playHead->getCurrentPosition(cpi);

        auto ppqPos = cpi.ppqPosition;
        const double timeSigNumerator = cpi.timeSigNumerator;

        static constexpr double loopMultipliers[] = { 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0 };
        auto* loopRateParam = parameters.getRawParameterValue("loopRate");

        int loopRateIndex = (loopRateParam != nullptr) ? (int)(*loopRateParam) : 2;
        double loopMultiplier = loopMultipliers[juce::jlimit(0, 6, loopRateIndex)];
        double beatsPerLoop = timeSigNumerator / loopMultiplier;
        double phase = std::fmod(ppqPos, beatsPerLoop) / beatsPerLoop;

        int curveIndex = juce::jlimit(0, static_cast<int>(pitchCurve.size()) - 1,
                                      static_cast<int>(phase * pitchCurve.size()));

        float normX = static_cast<float>(curveIndex) / static_cast<float>(pitchCurve.size() - 1);
        bool isMuted = std::any_of(erasedRanges.begin(), erasedRanges.end(),
                                   [normX](const auto& range) {
                                       return normX >= range.first && normX <= range.second;
                                   });

        float pitch = pitchCurve[curveIndex].pitch;
        float snap = *parameters.getRawParameterValue("snap");
        float quantPitch = quantizePitch(pitch, snap);
        smoothedPitch.setTargetValue(quantPitch);

        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);

            for (int i = 0; i < numSamples; ++i)
            {
                float in = channelData[i];

                if (isMuted)
                {
                    // Apply a soft fade (linear for now)
                    float fadeAmount = juce::jmap((float)i, 0.0f, (float)numSamples, 1.0f, 0.0f);
                    fadeAmount = juce::jlimit(0.0f, 1.0f, fadeAmount);
                    channelData[i] = in * fadeAmount;
                }
                else
                {
                    float pitchVal = smoothedPitch.getNextValue();
                    pitchShiftEffect.setPitch(pitchVal);
                    channelData[i] = pitchShiftEffect.processSample(in, ch);
                }
            }
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
void GrannyDrawAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    juce::XmlElement xmlState("DrawState");

    if (auto paramsXml = state.createXml())
        xmlState.addChildElement(paramsXml.release());

    auto* curveElement = new juce::XmlElement("PitchCurve");
    for (const auto& point : pitchCurve)
    {
        auto* p = curveElement->createNewChildElement("Point");
        p->setAttribute("x", point.normalizedX);
        p->setAttribute("pitch", point.pitch);
    }

    xmlState.addChildElement(curveElement);

    copyXmlToBinary(xmlState, destData);
}

void GrannyDrawAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (auto* paramsXml = xmlState->getChildByName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*paramsXml));

        if (auto* curveElement = xmlState->getChildByName("PitchCurve"))
        {
            pitchCurve.clear();
            for (auto* pointElement : curveElement->getChildIterator())
            {
                if (pointElement->hasTagName("Point"))
                {
                    CurvePoint cp;
                    cp.normalizedX = (float)pointElement->getDoubleAttribute("x", 0.0);
                    cp.pitch = (float)pointElement->getDoubleAttribute("pitch", 0.0);
                    pitchCurve.push_back(cp);
                }
            }
        }
    }
    
    needsCurveUpdate = true;

}

void GrannyDrawAudioProcessor::setPitchCurve(const std::vector<CurvePoint>& newCurve)
{
    pitchCurve = newCurve;
}

std::vector<CurvePoint> GrannyDrawAudioProcessor::getPitchCurve() const
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

void GrannyDrawAudioProcessor::setErasedRanges(const std::vector<std::pair<float, float>>& newRanges)
{
    erasedRanges = newRanges;
    
//    DBG("Erased ranges updated:");
//        for (const auto& range : erasedRanges)
//            DBG("Range: " << range.first << " to " << range.second);
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrannyDrawAudioProcessor();
}
