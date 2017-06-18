
#include <app/app_version.h>
#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
//#include <qtsingleapplication.h>

#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLoggingCategory>
#include <QSettings>
#include <QStyle>
#include <QTextStream>
#include <QThreadPool>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#include <QVariant>

#include <QApplication>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTemporaryDir>

using namespace ExtensionSystem;

enum { OptionIndent = 4, DescriptionIndent = 34 };

const char appNameC[] = "MVR UI";
const char corePluginNameC[] = "Core";
static const char *SHARE_PATH = "/../share/creator";

typedef QList<PluginSpec *> PluginSpecSet;

// Helpers for displaying messages. Note that there is no console on Windows.

// Format as <pre> HTML
static inline QString toHtml(const QString &t)
{
    QString res = t;
    res.replace(QLatin1Char('&'), QLatin1String("&amp;"));
    res.replace(QLatin1Char('<'), QLatin1String("&lt;"));
    res.replace(QLatin1Char('>'), QLatin1String("&gt;"));
    res.insert(0, QLatin1String("<html><pre>"));
    res.append(QLatin1String("</pre></html>"));
    return res;
}

static void displayHelpText(const QString &t)
{
    qWarning("%s", qPrintable(t));
}

static void printVersion(const PluginSpec *coreplugin)
{
    QString version;
    QTextStream str(&version);
    str << '\n' << appNameC << ' ' << coreplugin->version()<< " based on Qt " << qVersion() << "\n\n";
    PluginManager::formatPluginVersions(str);
    str << '\n' << coreplugin->copyright() << '\n';
    displayHelpText(version);
}

static inline void msgCoreLoadFailure(const QString &why)
{
    qCritical("%s", qPrintable(why));
}

static inline QStringList getPluginPaths()
{
    QStringList rc;

    // Figure out root:  Up one from 'bin'
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    const QString rootDirPath = rootDir.canonicalPath();
    QString pluginPath;

    // 2) "plugins" (Win/Linux)
    pluginPath = rootDirPath;
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String(IDE_LIBRARY_BASENAME);
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String(Core::Constants::IDE_TARGET);
    pluginPath += QLatin1String("/plugins");
    rc.push_back(pluginPath);

    // 3) <localappdata>/plugins/<ideversion>
    //    where <localappdata> is e.g.
    //    "%LOCALAPPDATA%\QtProject\IDE_TARGET" on Windows Vista and later
    //    "$XDG_DATA_HOME/data/QtProject/IDE_TARGET" or "~/.local/share/data/QtProject/IDE_TARGET" on Linux
    //    "~/Library/Application Support/QtProject/Qt Creator" on Mac
    pluginPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
#ifdef Q_OS_UNIX
    pluginPath += QLatin1String("/data");
#endif
    pluginPath += QLatin1Char('/')
            + QLatin1String(Core::Constants::IDE_SETTINGSVARIANT_STR)
            //+ QLatin1String("QtProject")
            + QLatin1Char('/');
    pluginPath += QLatin1String(Core::Constants::IDE_TARGET);
    pluginPath += QLatin1String("/plugins/");
    pluginPath += QLatin1String(Core::Constants::IDE_VERSION_LONG);
    //pluginPath += QLatin1String("V4.2.1");
    rc.push_back(pluginPath);

    return rc;
}

int main(int argc, char **argv)
{
    QLoggingCategory::setFilterRules(QLatin1String("qtc.*.debug=false\nqtc.*.info=false"));

    QApplication app(argc, argv);

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(qMax(4, 2 * threadCount));

    // Internationalization
    QTranslator translator;
    QTranslator qtTranslator;
    QStringList uiLanguages;
    uiLanguages = QLocale::system().uiLanguages();
    qDebug() << "uiLanguages: " << uiLanguages;

    const QString &creatorTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String(SHARE_PATH) + QLatin1String("/i18n");
    qDebug() << "creatorTrPath: " << creatorTrPath;
    foreach (QString locale, uiLanguages) {
        locale = QLocale(locale).name();
        qDebug() << "locale: " << locale;
        if (translator.load(locale, creatorTrPath)) {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            qDebug() << "qtTrPathFile: " << qtTrPath << "" << qtTrFile;
            // Binary installer puts Qt tr files into creatorTrPath
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, creatorTrPath)) {
                qDebug() << "installTranslator";
                app.installTranslator(&translator);
                app.installTranslator(&qtTranslator);
                app.setProperty("qtc_locale", locale);
                break;
            }
            //translator.load(QString()); // unload()
        } else if (locale == QLatin1String("C") /* overrideLanguage == "English" */) {
            // use built-in
            break;
        } else if (locale.startsWith(QLatin1String("en")) /* "English" is built-in */) {
            // use built-in
            break;
        }
    }

    PluginManager pluginManager;
    PluginManager::setPluginIID(QLatin1String("org.hik.mt.mvr.plugin"));

    // Load
    const QStringList pluginPaths = getPluginPaths();
    PluginManager::setPluginPaths(pluginPaths);


    qDebug() << QObject::tr("pluginPaths") << pluginPaths;

    const PluginSpecSet plugins = PluginManager::plugins();
    PluginSpec *coreplugin = 0;
    foreach (PluginSpec *spec, plugins) {
        if (spec->name() == QLatin1String(corePluginNameC)) {
            coreplugin = spec;
            printVersion(coreplugin);
            break;
        }
    }

    qDebug() << QObject::tr("coreplugin");
    if (!coreplugin) {
        qCritical("Could not find Core plugin");
        return 1;
    }
    if (!coreplugin->isEffectivelyEnabled()) {
        qCritical("Core plugin is disabled.");
        return 1;
    }
    if (coreplugin->hasError()) {
        qCritical("Core plugin is error: %s", qPrintable(coreplugin->errorString()));
        return 1;
    }

    PluginManager::loadPlugins();
    if (coreplugin->hasError()) {
        qCritical("Core plugin is error: %s", qPrintable(coreplugin->errorString()));
        return 1;
    }

    // shutdown plugin manager on the exit
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &pluginManager, &PluginManager::shutdown);

    return app.exec();
}
