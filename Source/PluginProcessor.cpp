#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorEditor* RS40AudioProcessor::createEditor()
{
    return new RS40AudioProcessorEditor(*this);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RS40AudioProcessor();
}
