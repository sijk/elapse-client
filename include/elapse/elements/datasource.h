#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
class QByteArray;

namespace elapse {

class OfflineDataSource;
class OfflineDataSourcePrivate;


/*!
 * \brief The DataSource interface is implemented by elements that receive data
 * from the device over the network.
 *
 * Each Pipeline has one DataSource which provides data to several
 * SampleDecoder%s.
 *
 * \ingroup pipeline-elements
 */

class DataSource : public QObject
{
    Q_OBJECT
public:
    /*! Construct a new DataSource as a child of the given \a parent. */
    explicit DataSource(QObject *parent = nullptr) : QObject(parent) {}

    bool isOfflineSource() const;

signals:
    /*! Emitted when video \a data is available. */
    void videoReady(QByteArray data);

    /*! Emitted when EEG \a data is available. */
    void eegReady(QByteArray data);

    /*! Emitted when IMU \a data is available. */
    void imuReady(QByteArray data);

    /*!
     * Emitted when the DataSource has succesfully connected to the device
     * and is ready to receive data.
     * \sa start()
     */
    void started();

    /*!
     * Emitted when an error occurs. The \a message is a human-readable string
     * which should be useful for debugging the error.
     */
    void error(QString message);

public slots:
    /*!
     * Start receiving data from the device. Classes inheriting from DataSource
     * must implement this method and ensure that started() is emitted at the
     * appropriate time.
     * \sa started()
    */
    virtual void start() = 0;

    /*!
     * Stop receiving data from the device. Classes inheriting from DataSource
     * must implement this method.
    */
    virtual void stop() = 0;
};



class ConfigManager
{
public:
    virtual QVariant get(const QString &subSystem, const QString &property) = 0;
};


class OfflineDataSource : public DataSource, public ConfigManager
{
    Q_OBJECT
public:
    explicit OfflineDataSource(QObject *parent = nullptr);
    ~OfflineDataSource();

protected:
    void exposeDeviceInterface();

private:
    OfflineDataSourcePrivate *d_ptr;
    Q_DECLARE_PRIVATE(OfflineDataSource)
};

} // namespace elapse

#endif // DATASOURCE_H
