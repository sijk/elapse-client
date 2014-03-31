#include <QVariant>
#include <QxtLogger>
#include "datasink.h"
#include "pipeline.h"

/*!
 * \page pipeline-arch Signal Pipeline Structure
 *
 * @startuml{pipeline-arch-class.png}
 *
 * hide class circle
 * hide class attributes
 * skinparam packageBorderColor white
 * skinparam shadowing false
 *
 * class dataSource {
 *      eegReady()
 *      videoReady()
 *      imuReady()
 * }
 * package {
 *      class "sampleDecoders[EEG]" {
 *          onData()
 *          newSample()
 *      }
 *      class "sampleDecoders[VIDEO]" {
 *          onData()
 *          newSample()
 *      }
 *      class "sampleDecoders[IMU]" {
 *          onData()
 *          newSample()
 *      }
 * }
 * package {
 *      class "featureExtractors[EEG]" {
 *          onSample()
 *          newFeatures()
 *      }
 *      class "featureExtractors[VIDEO]" {
 *          onSample()
 *          newFeatures()
 *      }
 *      class "featureExtractors[IMU]" {
 *          onSample()
 *          newFeatures()
 *      }
 * }
 * class classifier {
 *      onFeatures()
 *      newState()
 * }
 *
 * dataSource -> "sampleDecoders[EEG]" : QByteArray
 * dataSource -> "sampleDecoders[VIDEO]" : QByteArray
 * dataSource -> "sampleDecoders[IMU]" : QByteArray
 * "sampleDecoders[EEG]" -> "featureExtractors[EEG]" : EegSample
 * "sampleDecoders[VIDEO]" -> "featureExtractors[VIDEO]" : VideoSample
 * "sampleDecoders[IMU]" -> "featureExtractors[IMU]" : ImuSample
 * "featureExtractors[EEG]" -> classifier : FeatureVector
 * "featureExtractors[VIDEO]" -> classifier : FeatureVector
 * "featureExtractors[IMU]" -> classifier : FeatureVector
 *
 * @enduml
 */

/*!
 * Construct a new Pipeline as a child of the given \a parent.
 */
Pipeline::Pipeline(QObject *parent) :
    QObject(parent),
    _elements(nullptr),
    dataSink(new DataSink(this))
{
    qRegisterMetaType<elapse::SamplePtr>("SamplePtr");
}

/*!
 * \return the Pipeline's ElementSet.
 */
ElementSetPtr Pipeline::elements() const
{
    return _elements;
}

/*!
 * Set the window length of all of the FeatureExtractor%s in \a ms.
 */
void Pipeline::setWindowLength(uint ms)
{
    foreach(auto featureExtractor, _elements->featureExtractors)
        featureExtractor->setWindowLength(ms);
}

/*!
 * Set the window step of all of the FeatureExtractor%s in \a ms.
 */
void Pipeline::setWindowStep(uint ms)
{
    foreach(auto featureExtractor, _elements->featureExtractors)
        featureExtractor->setWindowStep(ms);
}

/*!
 * Provide a new set of elements to the Pipeline.
 *
 * This method also sets up the connections between the elements and propagates
 * signals from elements to the Pipeline.
 *
 * ElementSet lifetimes are handled automatically, so if you call setElements()
 * when the Pipeline already has an ElementSet the old set will be released
 * (and destroyed if nothing else holds a reference to it) and replaced by the
 * new set.
 */
void Pipeline::setElements(ElementSetPtr newElements)
{
    _elements = newElements;

    // Connect elements
    connect(_elements->dataSource, SIGNAL(eegReady(QByteArray)),
            _elements->sampleDecoders[Signal::EEG], SLOT(onData(QByteArray)));
    connect(_elements->dataSource, SIGNAL(videoReady(QByteArray)),
            _elements->sampleDecoders[Signal::VIDEO], SLOT(onData(QByteArray)));
    connect(_elements->dataSource, SIGNAL(imuReady(QByteArray)),
            _elements->sampleDecoders[Signal::IMU], SLOT(onData(QByteArray)));

    connect(_elements->sampleDecoders[Signal::EEG], SIGNAL(newSample(elapse::SamplePtr)),
            _elements->featureExtractors[Signal::EEG], SLOT(onSample(elapse::SamplePtr)));
    connect(_elements->sampleDecoders[Signal::VIDEO], SIGNAL(newSample(elapse::SamplePtr)),
            _elements->featureExtractors[Signal::VIDEO], SLOT(onSample(elapse::SamplePtr)));
    connect(_elements->sampleDecoders[Signal::IMU], SIGNAL(newSample(elapse::SamplePtr)),
            _elements->featureExtractors[Signal::IMU], SLOT(onSample(elapse::SamplePtr)));

    connect(_elements->featureExtractors[Signal::EEG], SIGNAL(newFeatures(elapse::FeatureVector)),
            _elements->classifier, SLOT(onFeatures(elapse::FeatureVector)));
    connect(_elements->featureExtractors[Signal::VIDEO], SIGNAL(newFeatures(elapse::FeatureVector)),
            _elements->classifier, SLOT(onFeatures(elapse::FeatureVector)));
    connect(_elements->featureExtractors[Signal::IMU], SIGNAL(newFeatures(elapse::FeatureVector)),
            _elements->classifier, SLOT(onFeatures(elapse::FeatureVector)));

    // Propagate signals from elements
    connect(_elements->dataSource, SIGNAL(started()), SIGNAL(started()));
    connect(_elements->dataSource, SIGNAL(error(QString)), SIGNAL(error(QString)));

    // Connect DataSink
    dataSink->setDelegate(_elements->dataSink);

    connect(_elements->dataSource, SIGNAL(eegReady(QByteArray)),
            dataSink, SLOT(onData(QByteArray)));
    connect(_elements->dataSource, SIGNAL(videoReady(QByteArray)),
            dataSink, SLOT(onData(QByteArray)));
    connect(_elements->dataSource, SIGNAL(imuReady(QByteArray)),
            dataSink, SLOT(onData(QByteArray)));
    connect(_elements->sampleDecoders[Signal::EEG], SIGNAL(newSample(elapse::SamplePtr)),
            dataSink, SLOT(onSample(elapse::SamplePtr)));
    connect(_elements->sampleDecoders[Signal::VIDEO], SIGNAL(newSample(elapse::SamplePtr)),
            dataSink, SLOT(onSample(elapse::SamplePtr)));
    connect(_elements->sampleDecoders[Signal::IMU], SIGNAL(newSample(elapse::SamplePtr)),
            dataSink, SLOT(onSample(elapse::SamplePtr)));
    connect(_elements->featureExtractors[Signal::EEG], SIGNAL(newFeatures(elapse::FeatureVector)),
            dataSink, SLOT(onFeatureVector(elapse::FeatureVector)));
    connect(_elements->featureExtractors[Signal::VIDEO], SIGNAL(newFeatures(elapse::FeatureVector)),
            dataSink, SLOT(onFeatureVector(elapse::FeatureVector)));
    connect(_elements->featureExtractors[Signal::IMU], SIGNAL(newFeatures(elapse::FeatureVector)),
            dataSink, SLOT(onFeatureVector(elapse::FeatureVector)));
    connect(_elements->classifier, SIGNAL(newState(elapse::CognitiveState)),
            dataSink, SLOT(onCognitiveState(elapse::CognitiveState)));
}

/*!
 * Start the pipeline.
 */
void Pipeline::start()
{
    Q_ASSERT(_elements);

    connect(_elements->sampleDecoders[Signal::EEG], SIGNAL(newSample(elapse::SamplePtr)),
            SLOT(setStartTime(elapse::SamplePtr)));

    qxtLog->info("Starting pipeline");
    if (!dataSink->start()) {
        emit error();
        return;
    }
    _elements->classifier->reset();
    _elements->dataSource->start();
}

/*!
 * Stop the pipeline.
 */
void Pipeline::stop()
{
    if (!_elements)
        return;

    qxtLog->info("Stopping pipeline");
    _elements->dataSource->stop();
    dataSink->stop();
    emit stopped();

    disconnect(_elements->sampleDecoders[Signal::EEG],
               SIGNAL(newSample(elapse::SamplePtr)), this, 0);
}

/*!
 * Called when the first \a sample is decoded. Sets the start timestamp for
 * the FeatureExtractor%s to one second after that to give all of the sensors
 * time to start up.
 */
void Pipeline::setStartTime(elapse::SamplePtr sample)
{
    quint64 startTime = sample->timestamp + 1e9;

    qxtLog->debug("Setting start time to", startTime / 1e9);
    foreach (auto featureExtractor, _elements->featureExtractors)
        featureExtractor->setStartTime(startTime);

    disconnect(_elements->sampleDecoders[Signal::EEG],
               SIGNAL(newSample(elapse::SamplePtr)), this, 0);
}

/*!
 * \fn Pipeline::started()
 * Emitted when the elapse::DataSource has started receiving data from
 * the device.
 */

/*!
 * \fn Pipeline::stopped()
 * Emitted when data has stopped flowing through the pipeline for any reason.
 */

/*!
 * \fn Pipeline::error(QString msg)
 * Emitted when an error has occurred somewhere in the pipeline. The \a msg is
 * human readable, suitable for displaying in a dialog box or similar.
 */
