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
    smoothedPitch.reset(sampleRate, 0.1);
    previousMuteStates.resize(getTotalNumInputChannels(), false);
    muteGains.resize(getTotalNumInputChannels(), 1.0f);
    muteStates.clear();
    muteStates.resize(getTotalNumInputChannels());
    for (auto& m : muteStates)
    {
        m.isMuted = false;
        m.muteCounter = 0;
    }



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


bool GrannyDrawAudioProcessor::isInErasedRange(float normalizedPhase)
{
    if (resampledCurve.size() < 2)
        return false;

    float floatIndex = normalizedPhase * (resampledCurve.size() - 1);
    int indexA = static_cast<int>(std::floor(floatIndex));
    int indexB = std::min(indexA + 1, static_cast<int>(resampledCurve.size() - 1));
    float t = floatIndex - indexA;

    float pitch = juce::jmap(t, 0.0f, 1.0f, resampledCurve[indexA].pitch, resampledCurve[indexB].pitch);

    for (const auto& region : erasedRanges)
    {
        if (normalizedPhase >= region.xMin && normalizedPhase <= region.xMax &&
            pitch           >= region.yMin && pitch           <= region.yMax)
        {
            return true;
        }
    }

    return false;
}



void GrannyDrawAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);
    
    if (resampledCurve.empty())
        return;
    
    playHead = getPlayHead();
    if (playHead != nullptr && playHead->getCurrentPosition(cpi))
    {
        auto ppqPos = cpi.ppqPosition;
        const double timeSigNumerator = cpi.timeSigNumerator;
        
        static constexpr double loopMultipliers[] = { 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0 };
        auto* loopRateParam = parameters.getRawParameterValue("loopRate");
        
        int loopRateIndex = (loopRateParam != nullptr) ? (int)(*loopRateParam) : 2;
        double loopMultiplier = loopMultipliers[juce::jlimit(0, 6, loopRateIndex)];
        double beatsPerLoop = timeSigNumerator / loopMultiplier;
        double phase = std::fmod(ppqPos, beatsPerLoop) / beatsPerLoop;
        playheadPhase.store(static_cast<float>(phase));
        
        
        float normPhase = static_cast<float>(phase);
        bool isMuted = isInErasedRange(normPhase);
        DBG("Playhead Phase: " << normPhase << " -> Muted? " << (isMuted ? "YES" : "NO"));


        float targetNormX = static_cast<float>(phase);
        float minX = resampledCurve.front().normalizedX;
        float maxX = resampledCurve.back().normalizedX;
        targetNormX = juce::jmap(targetNormX, 0.0f, 1.0f, minX, maxX);
        
            if (resampledCurve.size() >= 2)
            {
                float floatIndex = normPhase * (resampledCurve.size() - 1);
                int indexA = static_cast<int>(std::floor(floatIndex));
                int indexB = std::min(indexA + 1, static_cast<int>(resampledCurve.size() - 1));
                float t = floatIndex - indexA;
                
                float pitchA = resampledCurve[indexA].pitch;
                float pitchB = resampledCurve[indexB].pitch;
                float interpolatedPitch = juce::jmap(t, 0.0f, 1.0f, pitchA, pitchB);
                
                setPitchPlayheadIndex(indexA);
                
                float snap = *parameters.getRawParameterValue("snap");
                float quantPitch = quantizePitch(interpolatedPitch, snap);
                smoothedPitch.setTargetValue(quantPitch);
            }
        
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            float& gain = muteGains[ch];
            float targetGain = isMuted ? 0.0f : 1.0f;
            float gainStep = (targetGain - gain) / (float)numSamples;
            
            auto* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float in = channelData[i];
                
                float pitchVal = smoothedPitch.getNextValue();
                pitchShiftEffect.setPitch(pitchVal);
                
                float processed = pitchShiftEffect.processSample(in, ch);
                gain += gainStep;
                gain = std::clamp(gain, 0.0f, 1.0f);
                channelData[i] = processed * gain;
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

    auto* erasedElement = new juce::XmlElement("ErasedRanges");
    for (const auto& range : erasedRanges)
    {
        auto* r = erasedElement->createNewChildElement("Range");
        r->setAttribute("start", range.xMin);
        r->setAttribute("end", range.xMax);
        r->setAttribute("yMin", range.yMin);
        r->setAttribute("yMax", range.yMax);
    }
    xmlState.addChildElement(erasedElement);

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
            resampledCurve.clear();
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

        if (auto* erasedElement = xmlState->getChildByName("ErasedRanges"))
        {
            erasedRanges.clear();
            for (auto* rangeElement : erasedElement->getChildIterator())
            {
                if (rangeElement->hasTagName("Range"))
                {
                    float start = (float)rangeElement->getDoubleAttribute("start", 0.0);
                    float end   = (float)rangeElement->getDoubleAttribute("end", 0.0);
                    float yMin  = (float)rangeElement->getDoubleAttribute("yMin", -12.0);
                    float yMax  = (float)rangeElement->getDoubleAttribute("yMax", 12.0);
                    erasedRanges.emplace_back(ErasedRegion{ start, end, yMin, yMax });
                }
            }
        }
    }

    needsCurveUpdate = true;
}

std::vector<CurvePoint> resamplePitchCurve(const std::vector<CurvePoint>& input, size_t numSamples = 512)
{
    std::vector<CurvePoint> resampled;

    if (input.size() < 2)
        return resampled;

    std::vector<CurvePoint> sortedInput = input;
    std::sort(sortedInput.begin(), sortedInput.end(), [](const CurvePoint& a, const CurvePoint& b) {
        return a.normalizedX < b.normalizedX;
    });

    for (size_t i = 0; i < numSamples; ++i)
    {
        float targetX = (float)i / (float)(numSamples - 1);

        // Find segment surrounding targetX
        for (size_t j = 1; j < sortedInput.size(); ++j)
        {
            const auto& ptA = sortedInput[j - 1];
            const auto& ptB = sortedInput[j];

            if (targetX >= ptA.normalizedX && targetX <= ptB.normalizedX)
            {
                float t = (targetX - ptA.normalizedX) / (ptB.normalizedX - ptA.normalizedX);
                float interpPitch = juce::jmap(t, 0.0f, 1.0f, ptA.pitch, ptB.pitch);
                resampled.push_back({ targetX, interpPitch });
                break;
            }
        }
    }

    return resampled;
}


void GrannyDrawAudioProcessor::setPitchCurve(const std::vector<CurvePoint>& newCurve)
{
    pitchCurve = newCurve;
    resampledCurve = resamplePitchCurve(newCurve);
}

std::vector<CurvePoint> GrannyDrawAudioProcessor::getPitchCurve() const
{
    return resampledCurve;
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
    return resampledCurve.size();
}

void GrannyDrawAudioProcessor::setErasedRanges(const std::vector<ErasedRegion>& newRanges)
{
    erasedRanges = newRanges;
    
    DBG("=== Erased Ranges Set ===");
        for (const auto& r : erasedRanges)
        {
            DBG("ErasedRange -> X:[" << r.xMin << ", " << r.xMax << "]  Y:[" << r.yMin << ", " << r.yMax << "]");
        }
}

const std::vector<CurvePoint>& GrannyDrawAudioProcessor::getResampledPitchCurve() const
{
    return resampledCurve;
}





//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrannyDrawAudioProcessor();
}
