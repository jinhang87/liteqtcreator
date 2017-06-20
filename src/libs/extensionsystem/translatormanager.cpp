#include "translatormanager.h"
#include "translatormanager_p.h"
#include <QDebug>
#include <QCoreApplication>
#include <QLibraryInfo>

using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

static Internal::TranslatorManagerPrivate *d = 0;
static const char *SHARE_PATH = "/../share/creator";

TranslatorManager::TranslatorManager(QObject *parent) : QObject(parent)
{
    d = new TranslatorManagerPrivate(this);
}

TranslatorManager *TranslatorManager::instance()
{
    static TranslatorManager instance;
    return &instance;
}

void TranslatorManager::load()
{
    d->load();
}

bool TranslatorManager::changeLanguage(const QString& name)
{
    return d->changeLanguage(name);
}

TranslatorManagerPrivate::TranslatorManagerPrivate(TranslatorManager *translatorManager):
    q(translatorManager)
{
    //m_translaors = new QTranslator(this);
    //m_qtTranslaors = new QTranslator(this);
}

TranslatorManagerPrivate::~TranslatorManagerPrivate()
{

}


void TranslatorManagerPrivate::load()
{
    // Internationalization
    QStringList uiLanguages;
    //uiLanguages = QLocale::system().uiLanguages();
    uiLanguages << QLatin1String("zh-CN") << QLatin1String("en-US"); //build-in

    qDebug() << "uiLanguages: " << uiLanguages;
    //QCoreApplication *app = QCoreApplication::instance();
    const QString &creatorTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String(SHARE_PATH) + QLatin1String("/i18n");
    qDebug() << "creatorTrPath: " << creatorTrPath;
    foreach (QString locale, uiLanguages) {
        locale = QLocale(locale).name();
        //locale = QLatin1String("zh_CN");
        qDebug() << "locale: " << locale;
        m_translators[locale] = new QTranslator(this);
        m_qtTranslaors[locale] = new QTranslator(this);
        if (m_translators[locale]->load(locale, creatorTrPath)) {
            qDebug() << "installTranslator creator";
            //qApp->installTranslator(m_translaors);
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            qDebug() << "qtTrPathFile: " << qtTrPath << "" << qtTrFile;
            // Binary installer puts Qt tr files into creatorTrPath
            if (m_qtTranslaors[locale]->load(qtTrFile, qtTrPath) || m_qtTranslaors[locale]->load(qtTrFile, creatorTrPath)) {
                qDebug() << "installTranslator qt";
                //qApp->installTranslator(m_qtTranslaors);
                //qApp->setProperty("qtc_locale", locale);
                continue;
            }
            //translator.load(QString()); // unload()
        } else if (locale == QLatin1String("C") /* overrideLanguage == "English" */) {
            // use built-in
            continue;
        } else if (locale.startsWith(QLatin1String("en")) /* "English" is built-in */) {
            // use built-in
            continue;
        }
    }
    m_lastLanguage = QLatin1String("zh_CN");
    qApp->installTranslator(m_translators[m_lastLanguage]);
    qApp->installTranslator(m_qtTranslaors[m_lastLanguage]);
    qApp->setProperty("qtc_locale", m_lastLanguage);
}

bool TranslatorManagerPrivate::changeLanguage(const QString &name)
{
    qDebug() << "name = " << name;

    if(!m_translators[m_lastLanguage]->isEmpty())
        qApp->removeTranslator(m_translators[m_lastLanguage]);

    if(name != QLatin1String("en_US")){
        if(m_translators[name]->load(name, getTrPath())){
            qApp->installTranslator(m_translators[name]);
        }
    }else{
        if(m_translators[name]->load(name, "")){
            qApp->installTranslator(m_translators[name]);
        }
    }

    return true;
}

const QString TranslatorManagerPrivate::getTrPath()
{
    const QString creatorTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String(SHARE_PATH) + QLatin1String("/i18n");
    return creatorTrPath;
}

const QString TranslatorManagerPrivate::getQtTrPath()
{
    return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
}
