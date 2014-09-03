#include <QPluginLoader>
#include <QStaticPlugin>
#include <QxtLogger>
#include "native/util.h"
#include "staticpluginhost.h"

Q_IMPORT_PLUGIN(CorePlugin)

namespace elapse { namespace plugin {

/*!
 * Search for static plugins. The \a dir parameter is ignored since it is
 * meaningless in this case.
 */
QList<PluginData> StaticHost::searchForPluginsIn(QDir &dir)
{
    Q_UNUSED(dir);
    QList<PluginData> pluginData;
    QVector<QStaticPlugin> plugins = QPluginLoader::staticPlugins();

    for (const QStaticPlugin &plugin : plugins) {
        PluginData data;
        data.plugin.host = HostID::Static;

        if (!native::getPluginDataFrom(plugin, data)) {
            qxtLog->debug("Could not get static plugin instance for",
                          data.plugin.name);
            continue;
        }

        pluginData.append(data);
    }

    return pluginData;
}

/*!
 * Do nothing. File paths are not meaningful for statically-linked plugins.
 * See searchForPluginsIn() instead.
 */
PluginData StaticHost::getInfo(const QString &pluginPath)
{
    Q_UNUSED(pluginPath);
    return PluginData();
}

QObject *StaticHost::instantiateClass(const PluginInfo &pluginInfo,
                                      const ClassInfo &classInfo)
{
    QVector<QStaticPlugin> plugins = QPluginLoader::staticPlugins();

    for (const QStaticPlugin &plugin : plugins) {
        if (plugin.metaData()["className"].toString() != pluginInfo.name)
            continue;

        QObject *instance = native::instantiateClassFrom(plugin.instance(),
                                                         classInfo.className);
        if (instance)
            return instance;
        else
            qxtLog->debug("Could not instantiate", classInfo.className,
                          "from static plugin", pluginInfo.name);
    }

    return nullptr;
}

}} // namespace elapse::plugin
