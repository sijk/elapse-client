#include "dummyeegfeatureextractor.h"

namespace elapse { namespace coreplugin {

DummyEegFeatureExtractor::DummyEegFeatureExtractor()
{
}

void DummyEegFeatureExtractor::analyseSample(elapse::data::SamplePtr sample)
{
    sampleFlags[sample->timestamp] = 1;
}

std::vector<double> DummyEegFeatureExtractor::features()
{
    return { double(sampleFlags.size()) };
}

void DummyEegFeatureExtractor::removeDataBefore(elapse::time::Point time)
{
    sampleFlags.removeValuesBefore(time);
}

}} // namespace elapse::coreplugin
