#ifndef DECODER_H
#define DECODER_H

#include <QObject>
#include "elapse/sampletypes.h"

class QByteArray;

namespace elapse {

/*!
 * \brief The SampleDecoder class is the base class for elements that decode a
 * QByteArray to one or more Sample%s.
 *
 * Each Pipeline has several SampleDecoder%s: one per Signal::Type.
 *
 * \ingroup pipeline-elements
 */

class SampleDecoder : public QObject
{
    Q_OBJECT
public:
    /*! Construct a new SampleDecoder as a child of the given \a parent. */
    explicit SampleDecoder(QObject *parent = nullptr) : QObject(parent) {}

    /*! Destroy this SampleDecoder. */
    virtual ~SampleDecoder() {}

signals:
    /*! Emitted when a \a sample has been decoded. */
    void newSample(SamplePtr sample);

public slots:
    /*!
     * Executed when \a data is available for decoding. Classes that inherit
     * from SampleDecoder must implement this slot and ensure that newSample()
     * is emitted for every decoded sample. Note that there is not necessarily
     * a one-to-one mapping between the incoming \a data and outgoing Sample%s.
     */
    virtual void onData(QByteArray data) = 0;
};

} // namespace elapse

#endif // DECODER_H