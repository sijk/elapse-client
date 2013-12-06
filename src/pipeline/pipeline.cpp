#include <QVariant>
#include "elements.h"
#include "pipeline.h"

/*!
 * \page pipeline-arch Signal Pipeline Structure
 *
 * @startuml{pipeline-arch.png}
 *
 * [DataSource] -> [SampleDecoder]
 * [SampleDecoder] -> [FeatureExtractor]
 * [FeatureExtractor] -> [Classifier]
 *
 * @enduml
 */

/*!
 * Construct a new Pipeline as a child of the given \a parent.
 */
Pipeline::Pipeline(QObject *parent) :
    QObject(parent),
    _elements(nullptr)
{
}

/*!
 * Destroy this Pipeline and its ElementSet.
 */
Pipeline::~Pipeline()
{
    if (_elements)
        delete _elements;
}

/*!
 * \return the Pipeline's ElementSet.
 */
ElementSet *Pipeline::elements() const
{
    return _elements;
}

/*!
 * Provide a new set of elements to the Pipeline. The Pipeline takes ownership
 * of the given ElementSet and will destroy it when it is no longer needed.
 *
 * This method also sets up the connections between the elements and propagates
 * signals from elements to the Pipeline.
 *
 * Calling setElements() when the Pipeline already has an ElementSet will cause
 * the existing set to be destroyed and replaced by the new one.
 */
void Pipeline::setElements(ElementSet *newElements)
{
    if (_elements)
        delete _elements;

    _elements = newElements;

    // Connect elements
    connect(_elements->dataSource, SIGNAL(eegReady(QByteArray)),
            _elements->sampleDecoders[EEG], SLOT(onData(QByteArray)));

    // Propagate signals from elements
    connect(_elements->dataSource, SIGNAL(started()), SIGNAL(started()));
    connect(_elements->dataSource, SIGNAL(error(QString)), SIGNAL(error(QString)));
}

/*!
 * Start the pipeline.
 */
void Pipeline::start()
{
    if (!_elements) {
        emit error("The pipeline is not configured.");
        return;
    }

    _elements->dataSource->start();
}

/*!
 * Stop the pipeline.
 */
void Pipeline::stop()
{
    if (!_elements)
        return;

    _elements->dataSource->stop();
    emit stopped();
}

/*!
 * \fn Pipeline::started()
 * Emitted when the DataSource has started receiving data from the device.
 */

/*!
 * \fn Pipeline::stopped()
 * Emitted when data has stopped flowing through the pipeline for any reason.
 */

/*!
 * \fn Pipeline::error(const QString &msg)
 * Emitted when an error has occurred somewhere in the pipeline. The \a msg is
 * human readable, suitable for displaying in a dialog box or similar.
 */
