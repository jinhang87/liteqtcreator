
#pragma once

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QMenu;
class QTranslator;
QT_END_NAMESPACE


namespace Utils {
//class PathChooser;
//class Theme;
}


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

private:
    QHash<QString, QTranslator*> m_translators;
    QString m_lastLanguage;
};

