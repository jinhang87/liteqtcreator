
#pragma once

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE


namespace Utils {
//class PathChooser;
//class Theme;
}

namespace Core {

//class DesignMode;

namespace Internal {
class mainmenu;
//class EditMode;
//class Locator;

class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Core.json")

public:
    CorePlugin();
    ~CorePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    bool delayedInitialize();
    ShutdownFlag aboutToShutdown();
    QObject *remoteCommand(const QStringList & /* options */,
                           const QString &workingDirectory,
                           const QStringList &args);

public slots:
    void fileOpenRequest(const QString&);

private slots:
#if 0//defined(WITH_TESTS)
    void testVcsManager_data();
    void testVcsManager();
    void testSplitLineAndColumnNumber();
    void testSplitLineAndColumnNumber_data();
    // Locator:
    void test_basefilefilter();
    void test_basefilefilter_data();
#endif

private:
    //static void addToPathChooserContextMenu(Utils::PathChooser *pathChooser, QMenu *menu);

    mainmenu *m_mainmenu;
    //EditMode *m_editMode;
    //DesignMode *m_designMode;
    //Locator *m_locator;
};

} // namespace Internal
} // namespace Core
