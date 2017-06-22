

#include "coreplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>
#include "mainmenu.h"

#include <QtPlugin>
#include <QDebug>
#include <QDateTime>
#include <QMenu>
#include <QUuid>

using namespace Core;
using namespace Core::Internal;
using namespace Utils;

CorePlugin::CorePlugin()
{
    //qRegisterMetaType<Id>();
}

CorePlugin::~CorePlugin()
{

}

struct CoreArguments {
    QColor overrideColor;
    //Id themeId;
    bool presentationMode = false;
};

CoreArguments parseArguments(const QStringList &arguments)
{
    CoreArguments args;
    Q_UNUSED(arguments);
#if 0
    for (int i = 0; i < arguments.size(); ++i) {
        if (arguments.at(i) == QLatin1String("-color")) {
            const QString colorcode(arguments.at(i + 1));
            args.overrideColor = QColor(colorcode);
            i++; // skip the argument
        }
        if (arguments.at(i) == QLatin1String("-presentationMode"))
            args.presentationMode = true;
        if (arguments.at(i) == QLatin1String("-theme")) {
            args.themeId = Id::fromString(arguments.at(i + 1));
            i++; // skip the argument
        }
    }
#endif
    return args;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    qDebug() << "CorePlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    m_mainmenu.reset(new mainmenu);
    m_mainmenu->show();
    return true;
}

void CorePlugin::extensionsInitialized()
{
    qDebug() << "CorePlugin::extensionsInitialized";
    m_mainmenu->extensionsInitialized();
}

bool CorePlugin::delayedInitialize()
{
    return true;
}


void CorePlugin::fileOpenRequest(const QString &f)
{
    remoteCommand(QStringList(), QString(), QStringList(f));
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    //Find::aboutToShutdown();
    //m_mainWindow->aboutToShutdown();
    //delete m_mainmenu;
    m_mainmenu.reset();
    return SynchronousShutdown;
}
