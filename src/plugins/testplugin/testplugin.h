
#pragma once

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE


namespace Utils {
//class PathChooser;
//class Theme;
}


class TestPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.hik.mt.mvr.plugin" FILE "testplugin.json")

public:
    TestPlugin();
    ~TestPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    bool delayedInitialize();
    ShutdownFlag aboutToShutdown();

public slots:

private:

};

