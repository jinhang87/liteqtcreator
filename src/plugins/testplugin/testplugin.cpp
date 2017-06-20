

#include "testplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>

#include <QtPlugin>
#include <QDebug>
#include <QDateTime>
#include <QMenu>
#include <QUuid>

//using namespace Core;
using namespace Utils;

TestPlugin::TestPlugin()
{
    //qRegisterMetaType<Id>();
}

TestPlugin::~TestPlugin()
{

}

bool TestPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    qDebug() << "TestPlugin::initialize";
    return true;
}

void TestPlugin::extensionsInitialized()
{
    qDebug() << "TestPlugin::extensionsInitialized";
}

bool TestPlugin::delayedInitialize()
{
    return true;
}


ExtensionSystem::IPlugin::ShutdownFlag TestPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}
