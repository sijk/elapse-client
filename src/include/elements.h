#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "sampletypes.h"
#include "elements/datasource.h"
#include "elements/decoder.h"
class FeatureExtractor;
class Classifier;


/*!
 * \brief The ElementSet struct holds pointers to a complete set of
 * \ref signal-pipeline "pipeline elements".
 *
 * \ingroup signal-pipeline
 */

struct ElementSet
{
    DataSource *dataSource;
    SampleDecoder *sampleDecoders[N_SAMPLE_TYPES];
    FeatureExtractor *featureExtractors[N_SAMPLE_TYPES];
    Classifier *classifier;

    ~ElementSet();
};


/*!
 * Destroy this ElementSet and all of the elements it contains.
 */
inline ElementSet::~ElementSet() {
    delete dataSource;
    delete sampleDecoders[EEG];
//    for (int i = 0; i < N_SAMPLE_TYPES; i++) {
//        delete sampleDecoders[i];
//        delete featureExtractors[i];
//    }
//    delete classifier;
}

#endif // ELEMENTS_H
