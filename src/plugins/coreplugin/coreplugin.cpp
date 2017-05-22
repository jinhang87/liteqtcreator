/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "coreplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>

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
    #if 0
    if (ThemeEntry::availableThemes().isEmpty()) {
        *errorMessage = tr("No themes found in installation.");
        return false;
    }
    const CoreArguments args = parseArguments(arguments);
    Theme::initialPalette(); // Initialize palette before setting it
    Theme *themeFromArg = ThemeEntry::createTheme(args.themeId);
    setCreatorTheme(themeFromArg ? themeFromArg
                                 : ThemeEntry::createTheme(ThemeEntry::themeSetting()));
    new ActionManager(this);
    ActionManager::setPresentationModeEnabled(args.presentationMode);
    m_mainWindow = new MainWindow;
    if (args.overrideColor.isValid())
        m_mainWindow->setOverrideColor(args.overrideColor);
    m_locator = new Locator;
    qsrand(QDateTime::currentDateTime().toTime_t());
    const bool success = m_mainWindow->init(errorMessage);
    if (success) {
        m_editMode = new EditMode;
        addObject(m_editMode);
        ModeManager::activateMode(m_editMode->id());
        m_designMode = new DesignMode;
        InfoBar::initializeGloballySuppressed();
    }

    IWizardFactory::initialize();

    // Make sure we respect the process's umask when creating new files
    SaveFile::initializeUmask();

    Find::initialize();
    m_locator->initialize(this, arguments, errorMessage);

    MacroExpander *expander = Utils::globalMacroExpander();
    expander->registerVariable("CurrentDate:ISO", tr("The current date (ISO)."),
                               []() { return QDate::currentDate().toString(Qt::ISODate); });
    expander->registerVariable("CurrentTime:ISO", tr("The current time (ISO)."),
                               []() { return QTime::currentTime().toString(Qt::ISODate); });
    expander->registerVariable("CurrentDate:RFC", tr("The current date (RFC2822)."),
                               []() { return QDate::currentDate().toString(Qt::RFC2822Date); });
    expander->registerVariable("CurrentTime:RFC", tr("The current time (RFC2822)."),
                               []() { return QTime::currentTime().toString(Qt::RFC2822Date); });
    expander->registerVariable("CurrentDate:Locale", tr("The current date (Locale)."),
                               []() { return QDate::currentDate().toString(Qt::DefaultLocaleShortDate); });
    expander->registerVariable("CurrentTime:Locale", tr("The current time (Locale)."),
                               []() { return QTime::currentTime().toString(Qt::DefaultLocaleShortDate); });
    expander->registerVariable("Config:DefaultProjectDirectory", tr("The configured default directory for projects."),
                               []() { return DocumentManager::projectsDirectory(); });
    expander->registerVariable("Config:LastFileDialogDirectory", tr("The directory last visited in a file dialog."),
                               []() { return DocumentManager::fileDialogLastVisitedDirectory(); });
    expander->registerVariable("HostOs:isWindows", tr("Is Qt Creator running on Windows?"),
                               []() { return QVariant(Utils::HostOsInfo::isWindowsHost()).toString(); });
    expander->registerVariable("HostOs:isOSX", tr("Is Qt Creator running on OS X?"),
                               []() { return QVariant(Utils::HostOsInfo::isMacHost()).toString(); });
    expander->registerVariable("HostOs:isLinux", tr("Is Qt Creator running on Linux?"),
                               []() { return QVariant(Utils::HostOsInfo::isLinuxHost()).toString(); });
    expander->registerVariable("HostOs:isUnix", tr("Is Qt Creator running on any unix-based platform?"),
                               []() { return QVariant(Utils::HostOsInfo::isAnyUnixHost()).toString(); });
    expander->registerPrefix("CurrentDate:", tr("The current date (QDate formatstring)."),
                             [](const QString &fmt) { return QDate::currentDate().toString(fmt); });
    expander->registerPrefix("CurrentTime:", tr("The current time (QTime formatstring)."),
                             [](const QString &fmt) { return QTime::currentTime().toString(fmt); });
    expander->registerVariable("UUID", tr("Generate a new UUID."),
                               []() { return QUuid::createUuid().toString(); });

    expander->registerPrefix("#:", tr("A comment."), [](const QString &) { return QStringLiteral(""); });

    // Make sure all wizards are there when the user might access the keyboard shortcuts:
    connect(ICore::instance(), &ICore::optionsDialogRequested, []() { IWizardFactory::allWizardFactories(); });

    Utils::PathChooser::setAboutToShowContextMenuHandler(&CorePlugin::addToPathChooserContextMenu);

    return success;
    #else
    return true;
    #endif
}

void CorePlugin::extensionsInitialized()
{

}

bool CorePlugin::delayedInitialize()
{
    return true;
}

QObject *CorePlugin::remoteCommand(const QStringList & /* options */,
                                   const QString &workingDirectory,
                                   const QStringList &args)
{
    #if 0
    if (!ExtensionSystem::PluginManager::isInitializationDone()) {
        connect(ExtensionSystem::PluginManager::instance(), &ExtensionSystem::PluginManager::initializationDone,
                this, [this, workingDirectory, args]() {
                    remoteCommand(QStringList(), workingDirectory, args);
        });
        return nullptr;
    }
    IDocument *res = m_mainWindow->openFiles(
                args, ICore::OpenFilesFlags(ICore::SwitchMode | ICore::CanContainLineAndColumnNumbers),
                workingDirectory);
    m_mainWindow->raiseWindow();
    return res;
    #else
    return 0;
    #endif
}

void CorePlugin::fileOpenRequest(const QString &f)
{
    remoteCommand(QStringList(), QString(), QStringList(f));
}
    #if 0
void CorePlugin::addToPathChooserContextMenu(Utils::PathChooser *pathChooser, QMenu *menu)
{

    QList<QAction*> actions = menu->actions();
    QAction *firstAction = actions.isEmpty() ? nullptr : actions.first();

    auto *showInGraphicalShell = new QAction(Core::FileUtils::msgGraphicalShellAction(), menu);
    connect(showInGraphicalShell, &QAction::triggered, pathChooser, [pathChooser]() {
        Core::FileUtils::showInGraphicalShell(pathChooser, pathChooser->path());
    });
    menu->insertAction(firstAction, showInGraphicalShell);

    auto *showInTerminal = new QAction(Core::FileUtils::msgTerminalAction(), menu);
    connect(showInTerminal, &QAction::triggered, pathChooser, [pathChooser]() {
        Core::FileUtils::openTerminal(pathChooser->path());
    });
    menu->insertAction(firstAction, showInTerminal);

    if (firstAction)
        menu->insertSeparator(firstAction);

}
#endif
ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    //Find::aboutToShutdown();
    //m_mainWindow->aboutToShutdown();
    return SynchronousShutdown;
}
