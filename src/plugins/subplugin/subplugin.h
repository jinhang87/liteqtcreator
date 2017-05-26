
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

class SubPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.hik.mt.mvr.plugin" FILE "subplugin.json")

public:
    SubPlugin();
    ~SubPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    bool delayedInitialize();
    ShutdownFlag aboutToShutdown();

public slots:
    void fileOpenRequest(const QString&);



private:
    //static void addToPathChooserContextMenu(Utils::PathChooser *pathChooser, QMenu *menu);
    //mainmenu *m_mainmenu;

};

} // namespace Internal
} // namespace Core