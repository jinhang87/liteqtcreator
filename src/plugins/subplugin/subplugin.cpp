

#include "subplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/translatormanager.h>
#include <utils/algorithm.h>
#include "iomenuiconfactory.h"

#include <QtPlugin>
#include <QDebug>
#include <QDateTime>
#include <QMenu>
#include <QUuid>
#include <QTranslator>

using namespace Core;
using namespace Utils;
using namespace ExtensionSystem;

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
    qDebug() << "SubPlugin::initialize";
    return true;
}

void SubPlugin::extensionsInitialized()
{
    m_translators["en_US"] = new QTranslator(this);
    m_translators["zh_CN"] = new QTranslator(this);
    m_translators["zh_CN"]->load(":/i18n/ZH_CN.qm");
    connect(TranslatorManager::instance(), &TranslatorManager::languageChanged, [=](const QString &name){
        if(m_translators[m_lastLanguage] && !m_translators[m_lastLanguage]->isEmpty()){
            qApp->removeTranslator(m_translators[m_lastLanguage]);
        }
        if(m_translators[name] && !m_translators[name]->isEmpty()){
            qApp->installTranslator(m_translators[name]);
        }
        m_lastLanguage = name;
    });
    qDebug() << "SubPlugin::extensionsInitialized";
}

bool SubPlugin::delayedInitialize()
{
    return true;
}


ExtensionSystem::IPlugin::ShutdownFlag SubPlugin::aboutToShutdown()
{
    qDeleteAll(m_translators);
    return SynchronousShutdown;
}
