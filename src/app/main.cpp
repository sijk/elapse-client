#include <QApplication>
#include "pluginloader.h"
#include "tcpclienteegdatasource.h"
#include "eegdecoder.h"
#include "mainwindow.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    TcpClientEegDataSource source;
    EegDecoder eegdec;

    source.setProperty("host", "overo.local");
    eegdec.setProperty("gain", 1);
    eegdec.setProperty("vref", 4.5e6);

    // Connect EEG signal chain
    QObject::connect(&source, SIGNAL(eegReady(QByteArray)),
                     &eegdec, SLOT(onData(QByteArray)));
    QObject::connect(&eegdec, SIGNAL(newSample(EegSample)),
                     &w, SLOT(onDataReady(EegSample)));

    // Connect controls to source
    QObject::connect(&w, SIGNAL(start()), &source, SLOT(start()));
    QObject::connect(&w, SIGNAL(stop()), &source, SLOT(stop()));

    // Connect source signals to GUI
    QObject::connect(&source, SIGNAL(error(QString)),
                     &w, SLOT(onSourceError(QString)));
    QObject::connect(&source, SIGNAL(started()),
                     &w, SLOT(onSourceStarted()));

    w.show();
    return a.exec();
}
