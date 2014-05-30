#ifndef IMUDECODER_H
#define IMUDECODER_H

#include <elapse/elements/decoder.h>


/*!
 * \brief The ImuDecoder class decodes the byte stream from the IMU.
 *
 * \ingroup core-plugin
 */

class ImuDecoder : public elapse::SampleDecoder
{
    Q_OBJECT
    Q_CLASSINFO("SignalType", "IMU")

public:
    Q_INVOKABLE explicit ImuDecoder(QObject *parent = nullptr);

public slots:
    void onData(QByteArray data);
};

#endif // IMUDECODER_H
