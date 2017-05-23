

#pragma once

#include "iplugin.h"

#include <QString>

namespace ExtensionSystem {

class PluginManager;
class PluginSpec;

namespace Internal {

class IPluginPrivate
{
public:
    PluginSpec *pluginSpec;

    QList<QObject *> addedObjectsInReverseOrder;
};

} // namespace Internal
} // namespace ExtensionSystem
