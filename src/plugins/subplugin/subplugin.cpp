

#include "subplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>
#include "iomenuiconfactory.h"

#include <QtPlugin>
#include <QDebug>
#include <QDateTime>
#include <QMenu>
#include <QUuid>

using namespace Core;
using namespace Utils;

SubPlugin::SubPlugin()
{
    //qRegisterMetaType<Id>();
}

SubPlugin::~SubPlugin()
{

}

bool SubPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    addAutoReleasedObject(new ioMenuIconFactory);
    return true;
}

void SubPlugin::extensionsInitialized()
{

}

bool SubPlugin::delayedInitialize()
{
    return true;
}


ExtensionSystem::IPlugin::ShutdownFlag SubPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}
