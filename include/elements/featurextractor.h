#ifndef FEATUREXTRACTOR_H
#define FEATUREXTRACTOR_H

#include <QObject>

class Sample;


/*!
 * \brief The FeatureExtractor class is the base class for elements that
 * analyse a time series of \ref Sample "Samples" to extract a FeatureVector.
 *
 * Each Pipeline has several FeatureExtractors: one per \ref SampleType.
 *
 * \ingroup pipeline-elements
 */

class FeatureExtractor : public QObject
{
    Q_OBJECT

public:
    /*! Construct a new FeatureExtractor as a child of the given \a parent. */
    explicit FeatureExtractor(QObject *parent = 0) : QObject(parent) {}

    /*! Destroy this FeatureExtractor. */
    virtual ~FeatureExtractor() {}

public slots:
    /*! Executed when the next \a sample is available for analysis. */
    virtual void onSample(const Sample &sample) = 0;
};

#endif // FEATUREXTRACTOR_H
