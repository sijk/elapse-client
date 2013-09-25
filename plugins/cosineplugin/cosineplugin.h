#ifndef COSINEPROVIDERPLUGIN_H
#define COSINEPROVIDERPLUGIN_H

#include "plugin.h"
#include "cosineprovider.h"

class CosinePlugin : public DataProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DataProviderInterface_iid)
    Q_CLASSINFO("description", "DataProviders that provide cosine waves")
protected:
    ClassList classes()
    {
        return { CosineProvider::staticMetaObject };
    }
};

#endif // COSINEPROVIDERPLUGIN_H