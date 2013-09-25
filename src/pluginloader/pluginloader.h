#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QDir>
#include <QHash>
#include <QJsonObject>

class QStringList;

class PluginLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDir searchPath READ searchPath WRITE setSearchPath)

public:
    explicit PluginLoader(QObject *parent = 0);

    QFileInfoList files() const;
    QStringList interfaces() const;

    QFileInfoList filesForInterface(const QString &iid) const;
    QStringList keysForInterface(const QString &iid) const;

    QStringList keysForFile(const QFileInfo &file) const;
    QJsonObject infoForFile(const QFileInfo &file) const;

    QJsonObject infoForKey(const QString &key) const;
    QFileInfo fileForKey(const QString &key) const;

    QObject *create(const QString &key) const;
    template<class T> T create(const QString &key) const;

    QDir searchPath() const;

public slots:
    void setSearchPath(QDir path);

signals:
    void createdKey(const QString &key) const;

private:
    void rescan();
    QDir path;
    QHash<QString,QJsonObject> pluginInfo;
    QHash<QString,QFileInfo> pluginFile;
};


template<class T>
T PluginLoader::create(const QString &key) const
{
    return qobject_cast<T>(create(key));
}

#endif // PLUGINLOADER_H