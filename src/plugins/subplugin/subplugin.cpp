

#include "subplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>
//#include "mainmenu.h"

#include <QtPlugin>
#include <QDebug>
#include <QDateTime>
#include <QMenu>
#include <QUuid>

using namespace Core;
using namespace Core::Internal;
using namespace Utils;

SubPlugin::SubPlugin()
{
    //qRegisterMetaType<Id>();
}

SubPlugin::~SubPlugin()
{

}

struct CoreArguments {
    QColor overrideColor;
    //Id themeId;
    bool presentationMode = false;
};

bool SubPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    //m_mainmenu = new mainmenu;
    //m_mainmenu->show();
    return true;
}

void SubPlugin::extensionsInitialized()
{

}

bool SubPlugin::delayedInitialize()
{
    return true;
}


void SubPlugin::fileOpenRequest(const QString &f)
{
    remoteCommand(QStringList(), QString(), QStringList(f));
}

ExtensionSystem::IPlugin::ShutdownFlag SubPlugin::aboutToShutdown()
{
    //Find::aboutToShutdown();
    //m_mainWindow->aboutToShutdown();
    //delete m_mainmenu;
    return SynchronousShutdown;
}
