
#pragma once

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace Core {


namespace Internal {
    
class mainmenu;

class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.hik.mt.mvr.plugin" FILE "Core.json")

public:
    CorePlugin();
    ~CorePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    bool delayedInitialize();
    ShutdownFlag aboutToShutdown();

public slots:
    void fileOpenRequest(const QString&);

private:
    QScopedPointer<mainmenu> m_mainmenu;

};

} // namespace Internal
} // namespace Core
