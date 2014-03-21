#ifndef FOODUMMYSOURCE_H
#define FOODUMMYSOURCE_H

#include <QTimer>
#include <elapse/elements/datasource.h>


class FooDummySource : public elapse::DataSource
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FooDummySource(QObject *parent = nullptr);

public slots:
    void start();
    void stop();

private slots:
    void emitDummyData();

private:
    QTimer timer;
};

#endif // FOODUMMYSOURCE_H