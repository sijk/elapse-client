#ifndef IMUDECODER_H
#define IMUDECODER_H

#include "elements/decoder.h"
#include "displayable.h"

class HeadWidget;


/*!
 * \brief The ImuDecoder class decodes the byte stream from the IMU.
 *
 * \ingroup core-plugin
 */

class ImuDecoder : public SampleDecoder, public Displayable
{
    Q_OBJECT
    Q_CLASSINFO("SampleType", "IMU")

public:
    Q_INVOKABLE explicit ImuDecoder(QObject *parent = nullptr);

    QWidget *getWidget();

public slots:
    void onData(QByteArray data);

private:
    void updateHeadWidget(const ImuSample *sample);
    HeadWidget *headWidget;
};

#endif // IMUDECODER_H