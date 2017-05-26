

#include "pluginmanager.h"
#include "pluginmanager_p.h"
#include "pluginspec.h"
#include "pluginspec_p.h"
#include "iplugin.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QLibrary>
#include <QLibraryInfo>
#include <QMetaProperty>
#include <QSettings>
#include <QTextStream>
#include <QTime>
#include <QWriteLocker>
#include <QDebug>
#include <QTimer>
#include <QSysInfo>

#include <utils/algorithm.h>
//#include <utils/executeondestruction.h>
//#include <utils/hostosinfo.h>
#include <utils/qtcassert.h>
//#include <utils/synchronousprocess.h>

#include <functional>

Q_LOGGING_CATEGORY(pluginLog, "qtc.extensionsystem")

const char C_IGNORED_PLUGINS[] = "Plugins/Ignored";
const char C_FORCEENABLED_PLUGINS[] = "Plugins/ForceEnabled";
const int DELAYED_INITIALIZE_INTERVAL = 20; // ms

enum { debugLeaks = 0 };

using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;
using namespace Utils;

static Internal::PluginManagerPrivate *d = 0;
static PluginManager *m_instance = 0;

/*!
    Gets the unique plugin manager instance.
*/
PluginManager *PluginManager::instance()
{
    return m_instance;
}

/*!
    Creates a plugin manager. Should be done only once per application.
*/
PluginManager::PluginManager()
{
    m_instance = this;
    d = new PluginManagerPrivate(this);
}

/*!
    \internal
*/
PluginManager::~PluginManager()
{
    delete d;
    d = 0;
}

/*!
    Adds the object \a obj to the object pool, so it can be retrieved
    again from the pool by type.

    The plugin manager does not do any memory management - added objects
    must be removed from the pool and deleted manually by whoever is responsible for the object.

    Emits the objectAdded() signal.

    \sa PluginManager::removeObject()
    \sa PluginManager::getObject()
    \sa PluginManager::getObjects()
*/
void PluginManager::addObject(QObject *obj)
{
    d->addObject(obj);
}

/*!
    Emits aboutToRemoveObject() and removes the object \a obj from the object pool.
    \sa PluginManager::addObject()
*/
void PluginManager::removeObject(QObject *obj)
{
    d->removeObject(obj);
}

/*!
    Retrieves the list of all objects in the pool, unfiltered.

    Usually, clients do not need to call this function.

    \sa PluginManager::getObject()
    \sa PluginManager::getObjects()
*/
QList<QObject *> PluginManager::allObjects()
{
    return d->allObjects;
}

QReadWriteLock *PluginManager::listLock()
{
    return &d->m_lock;
}

/*!
    Tries to load all the plugins that were previously found when
    setting the plugin search paths. The plugin specs of the plugins
    can be used to retrieve error and state information about individual plugins.

    \sa setPluginPaths()
    \sa plugins()
*/
void PluginManager::loadPlugins()
{
    return d->loadPlugins();
}

/*!
    Returns true if any plugin has errors even though it is enabled.
    Most useful to call after loadPlugins().
*/
bool PluginManager::hasError()
{
    return Utils::anyOf(plugins(), [](PluginSpec *spec) {
        // only show errors on startup if plugin is enabled.
        return spec->hasError() && spec->isEffectivelyEnabled();
    });
}

/*!
    Returns all plugins that require \a spec to be loaded. Recurses into dependencies.
 */
QSet<PluginSpec *> PluginManager::pluginsRequiringPlugin(PluginSpec *spec)
{
    QSet<PluginSpec *> dependingPlugins({spec});
    // recursively add plugins that depend on plugins that.... that depend on spec
    foreach (PluginSpec *spec, d->loadQueue()) {
        if (spec->requiresAny(dependingPlugins))
            dependingPlugins.insert(spec);
    }
    dependingPlugins.remove(spec);
    return dependingPlugins;
}

/*!
    Returns all plugins that \a spec requires to be loaded. Recurses into dependencies.
 */
QSet<PluginSpec *> PluginManager::pluginsRequiredByPlugin(PluginSpec *spec)
{
    QSet<PluginSpec *> recursiveDependencies;
    recursiveDependencies.insert(spec);
    QList<PluginSpec *> queue;
    queue.append(spec);
    while (!queue.isEmpty()) {
        PluginSpec *checkSpec = queue.takeFirst();
        QHashIterator<PluginDependency, PluginSpec *> depIt(checkSpec->dependencySpecs());
        while (depIt.hasNext()) {
            depIt.next();
            if (depIt.key().type != PluginDependency::Required)
                continue;
            PluginSpec *depSpec = depIt.value();
            if (!recursiveDependencies.contains(depSpec)) {
                recursiveDependencies.insert(depSpec);
                queue.append(depSpec);
            }
        }
    }
    recursiveDependencies.remove(spec);
    return recursiveDependencies;
}

/*!
    Shuts down and deletes all plugins.
*/
void PluginManager::shutdown()
{
    d->shutdown();
}

/*!
    The list of paths were the plugin manager searches for plugins.

    \sa setPluginPaths()
*/
QStringList PluginManager::pluginPaths()
{
    return d->pluginPaths;
}

/*!
    Sets the plugin search paths, i.e. the file system paths where the plugin manager
    looks for plugin descriptions. All given \a paths and their sub directory trees
    are searched for plugin xml description files.

    \sa pluginPaths()
    \sa loadPlugins()
*/
void PluginManager::setPluginPaths(const QStringList &paths)
{
    d->setPluginPaths(paths);
}

/*!
    The IID that valid plugins must have.

    \sa setPluginIID()
*/
QString PluginManager::pluginIID()
{
    return d->pluginIID;
}

/*!
    Sets the IID that valid plugins must have. Only plugins with this IID are loaded, others are
    silently ignored.

    At the moment this must be called before setPluginPaths() is called.
    // ### TODO let this + setPluginPaths read the plugin meta data lazyly whenever loadPlugins() or plugins() is called.
*/
void PluginManager::setPluginIID(const QString &iid)
{
    d->pluginIID = iid;
}

/*!
    Defines the user specific settings to use for information about enabled and
    disabled plugins.
    Needs to be set before the plugin search path is set with setPluginPaths().
*/
void PluginManager::setSettings(QSettings *settings)
{
    d->setSettings(settings);
}

/*!
    Defines the global (user-independent) settings to use for information about
    default disabled plugins.
    Needs to be set before the plugin search path is set with setPluginPaths().
*/
void PluginManager::setGlobalSettings(QSettings *settings)
{
    d->setGlobalSettings(settings);
}

/*!
    Returns the user specific settings used for information about enabled and
    disabled plugins.
*/
QSettings *PluginManager::settings()
{
    return d->settings;
}

/*!
    Returns the global (user-independent) settings used for information about default disabled plugins.
*/
QSettings *PluginManager::globalSettings()
{
    return d->globalSettings;
}

void PluginManager::writeSettings()
{
    d->writeSettings();
}

/*!
    The arguments left over after parsing (that were neither startup nor plugin
    arguments). Typically, this will be the list of files to open.
*/
QStringList PluginManager::arguments()
{
    return d->arguments;
}

/*!
    List of all plugin specifications that have been found in the plugin search paths.
    This list is valid directly after the setPluginPaths() call.
    The plugin specifications contain the information from the plugins' xml description files
    and the current state of the plugins. If a plugin's library has been already successfully loaded,
    the plugin specification has a reference to the created plugin instance as well.

    \sa setPluginPaths()
*/
const QList<PluginSpec *> PluginManager::plugins()
{
    return d->pluginSpecs;
}

QHash<QString, QList<PluginSpec *> > PluginManager::pluginCollections()
{
    return d->pluginCategories;
}

static const char argumentKeywordC[] = ":arguments";
static const char pwdKeywordC[] = ":pwd";

/*!
    Serializes plugin options and arguments for sending in a single string
    via QtSingleApplication:
    ":myplugin|-option1|-option2|:arguments|argument1|argument2",
    as a list of lists started by a keyword with a colon. Arguments are last.

    \sa setPluginPaths()
*/
QString PluginManager::serializedArguments()
{
    const QChar separator = QLatin1Char('|');
    QString rc;
    foreach (const PluginSpec *ps, plugins()) {
        if (!ps->arguments().isEmpty()) {
            if (!rc.isEmpty())
                rc += separator;
            rc += QLatin1Char(':');
            rc += ps->name();
            rc += separator;
            rc +=  ps->arguments().join(separator);
        }
    }
    if (!rc.isEmpty())
        rc += separator;
    rc += QLatin1String(pwdKeywordC) + separator + QDir::currentPath();
    if (!d->arguments.isEmpty()) {
        if (!rc.isEmpty())
            rc += separator;
        rc += QLatin1String(argumentKeywordC);
        foreach (const QString &argument, d->arguments)
            rc += separator + argument;
    }
    return rc;
}

/* Extract a sublist from the serialized arguments
 * indicated by a keyword starting with a colon indicator:
 * ":a,i1,i2,:b:i3,i4" with ":a" -> "i1,i2"
 */
static QStringList subList(const QStringList &in, const QString &key)
{
    QStringList rc;
    // Find keyword and copy arguments until end or next keyword
    const QStringList::const_iterator inEnd = in.constEnd();
    QStringList::const_iterator it = std::find(in.constBegin(), inEnd, key);
    if (it != inEnd) {
        const QChar nextIndicator = QLatin1Char(':');
        for (++it; it != inEnd && !it->startsWith(nextIndicator); ++it)
            rc.append(*it);
    }
    return rc;
}

/*!
    Parses the options encoded by serializedArguments() const
    and passes them on to the respective plugins along with the arguments.

    \a socket is passed for disconnecting the peer when the operation is done (for example,
    document is closed) for supporting the -block flag.
*/

void PluginManager::remoteArguments(const QString &serializedArgument, QObject *socket)
{
    if (serializedArgument.isEmpty())
        return;
    QStringList serializedArguments = serializedArgument.split(QLatin1Char('|'));
    const QStringList pwdValue = subList(serializedArguments, QLatin1String(pwdKeywordC));
    const QString workingDirectory = pwdValue.isEmpty() ? QString() : pwdValue.first();
    const QStringList arguments = subList(serializedArguments, QLatin1String(argumentKeywordC));
    foreach (const PluginSpec *ps, plugins()) {
        if (ps->state() == PluginSpec::Running) {
            const QStringList pluginOptions = subList(serializedArguments, QLatin1Char(':') + ps->name());
            QObject *socketParent = ps->plugin()->remoteCommand(pluginOptions, workingDirectory,
                                                                arguments);
            if (socketParent && socket) {
                socket->setParent(socketParent);
                socket = 0;
            }
        }
    }
    if (socket)
        delete socket;
}


static inline void indent(QTextStream &str, int indent)
{
    str << QString(indent, ' ');
}

static inline void formatOption(QTextStream &str,
                                const QString &opt, const QString &parm, const QString &description,
                                int optionIndentation, int descriptionIndentation)
{
    int remainingIndent = descriptionIndentation - optionIndentation - opt.size();
    indent(str, optionIndentation);
    str << opt;
    if (!parm.isEmpty()) {
        str << " <" << parm << '>';
        remainingIndent -= 3 + parm.size();
    }
    if (remainingIndent >= 1) {
        indent(str, remainingIndent);
    } else {
        str << '\n';
        indent(str, descriptionIndentation);
    }
    str << description << '\n';
}

/*!
    Formats the version of the plugin specs for command line help.
*/
void PluginManager::formatPluginVersions(QTextStream &str)
{
    foreach (PluginSpec *ps, d->pluginSpecs)
        str << "  " << ps->name() << ' ' << ps->version() << ' ' << ps->description() <<  '\n';
}

/*!
 * \internal
 */
bool PluginManager::testRunRequested()
{
    return !d->testSpecs.isEmpty();
}

/*!
    Creates a profiling entry showing the elapsed time if profiling is
    activated.
*/

void PluginManager::profilingReport(const char *what, const PluginSpec *spec)
{
    d->profilingReport(what, spec);
}


/*!
    Returns a list of plugins in load order.
*/
QList<PluginSpec *> PluginManager::loadQueue()
{
    return d->loadQueue();
}

//============PluginManagerPrivate===========

/*!
    \internal
*/
PluginSpec *PluginManagerPrivate::createSpec()
{
    return new PluginSpec();
}

/*!
    \internal
*/
void PluginManagerPrivate::setSettings(QSettings *s)
{
    if (settings)
        delete settings;
    settings = s;
    if (settings)
        settings->setParent(this);
}

/*!
    \internal
*/
void PluginManagerPrivate::setGlobalSettings(QSettings *s)
{
    if (globalSettings)
        delete globalSettings;
    globalSettings = s;
    if (globalSettings)
        globalSettings->setParent(this);
}

/*!
    \internal
*/
PluginSpecPrivate *PluginManagerPrivate::privateSpec(PluginSpec *spec)
{
    return spec->d;
}

void PluginManagerPrivate::nextDelayedInitialize()
{
    while (!delayedInitializeQueue.isEmpty()) {
        PluginSpec *spec = delayedInitializeQueue.takeFirst();
        profilingReport(">delayedInitialize", spec);
        bool delay = spec->d->delayedInitialize();
        profilingReport("<delayedInitialize", spec);
        if (delay)
            break; // do next delayedInitialize after a delay
    }
    if (delayedInitializeQueue.isEmpty()) {
        m_isInitializationDone = true;
        delete delayedInitializeTimer;
        delayedInitializeTimer = 0;
        profilingSummary();
        emit q->initializationDone();
    } else {
        delayedInitializeTimer->start();
    }
}

/*!
    \internal
*/
PluginManagerPrivate::PluginManagerPrivate(PluginManager *pluginManager) :
    delayedInitializeTimer(0),
    shutdownEventLoop(0),
    m_profileElapsedMS(0),
    m_profilingVerbosity(0),
    settings(0),
    globalSettings(0),
    q(pluginManager)
{
}


/*!
    \internal
*/
PluginManagerPrivate::~PluginManagerPrivate()
{
    qDeleteAll(pluginSpecs);
}

/*!
    \internal
*/
void PluginManagerPrivate::writeSettings()
{
    if (!settings)
        return;
    QStringList tempDisabledPlugins;
    QStringList tempForceEnabledPlugins;
    foreach (PluginSpec *spec, pluginSpecs) {
        if (spec->isEnabledByDefault() && !spec->isEnabledBySettings())
            tempDisabledPlugins.append(spec->name());
        if (!spec->isEnabledByDefault() && spec->isEnabledBySettings())
            tempForceEnabledPlugins.append(spec->name());
    }

    settings->setValue(QLatin1String(C_IGNORED_PLUGINS), tempDisabledPlugins);
    settings->setValue(QLatin1String(C_FORCEENABLED_PLUGINS), tempForceEnabledPlugins);
}

/*!
    \internal
*/
void PluginManagerPrivate::readSettings()
{
    if (globalSettings) {
        defaultDisabledPlugins = globalSettings->value(QLatin1String(C_IGNORED_PLUGINS)).toStringList();
        defaultEnabledPlugins = globalSettings->value(QLatin1String(C_FORCEENABLED_PLUGINS)).toStringList();
    }
    if (settings) {
        disabledPlugins = settings->value(QLatin1String(C_IGNORED_PLUGINS)).toStringList();
        forceEnabledPlugins = settings->value(QLatin1String(C_FORCEENABLED_PLUGINS)).toStringList();
    }
}

/*!
    \internal
*/
void PluginManagerPrivate::stopAll()
{
    if (delayedInitializeTimer && delayedInitializeTimer->isActive()) {
        delayedInitializeTimer->stop();
        delete delayedInitializeTimer;
        delayedInitializeTimer = 0;
    }
    QList<PluginSpec *> queue = loadQueue();
    foreach (PluginSpec *spec, queue) {
        loadPlugin(spec, PluginSpec::Stopped);
    }
}

/*!
    \internal
*/
void PluginManagerPrivate::deleteAll()
{
    Utils::reverseForeach(loadQueue(), [this](PluginSpec *spec) {
        loadPlugin(spec, PluginSpec::Deleted);
    });
}

/*!
    \internal
*/
void PluginManagerPrivate::addObject(QObject *obj)
{
    {
        QWriteLocker lock(&m_lock);
        if (obj == 0) {
            qWarning() << "PluginManagerPrivate::addObject(): trying to add null object";
            return;
        }
        if (allObjects.contains(obj)) {
            qWarning() << "PluginManagerPrivate::addObject(): trying to add duplicate object";
            return;
        }

        if (debugLeaks)
            qDebug() << "PluginManagerPrivate::addObject" << obj << obj->objectName();

        if (m_profilingVerbosity && !m_profileTimer.isNull()) {
            // Report a timestamp when adding an object. Useful for profiling
            // its initialization time.
            const int absoluteElapsedMS = m_profileTimer->elapsed();
            qDebug("  %-43s %8dms", obj->metaObject()->className(), absoluteElapsedMS);
        }

        allObjects.append(obj);
    }
    emit q->objectAdded(obj);
}

/*!
    \internal
*/
void PluginManagerPrivate::removeObject(QObject *obj)
{
    if (obj == 0) {
        qWarning() << "PluginManagerPrivate::removeObject(): trying to remove null object";
        return;
    }

    if (!allObjects.contains(obj)) {
        qWarning() << "PluginManagerPrivate::removeObject(): object not in list:"
            << obj << obj->objectName();
        return;
    }
    if (debugLeaks)
        qDebug() << "PluginManagerPrivate::removeObject" << obj << obj->objectName();

    emit q->aboutToRemoveObject(obj);
    QWriteLocker lock(&m_lock);
    allObjects.removeAll(obj);
}

/*!
    \internal
*/
void PluginManagerPrivate::loadPlugins()
{
    QList<PluginSpec *> queue = loadQueue();
    foreach (PluginSpec *spec, queue) {
        loadPlugin(spec, PluginSpec::Loaded);
    }
    foreach (PluginSpec *spec, queue) {
        loadPlugin(spec, PluginSpec::Initialized);
    }
    Utils::reverseForeach(queue, [this](PluginSpec *spec) {
        loadPlugin(spec, PluginSpec::Running);
        if (spec->state() == PluginSpec::Running) {
            delayedInitializeQueue.append(spec);
        } else {
            // Plugin initialization failed, so cleanup after it
            spec->d->kill();
        }
    });
    emit q->pluginsChanged();

    delayedInitializeTimer = new QTimer;
    delayedInitializeTimer->setInterval(DELAYED_INITIALIZE_INTERVAL);
    delayedInitializeTimer->setSingleShot(true);
    connect(delayedInitializeTimer, &QTimer::timeout,
            this, &PluginManagerPrivate::nextDelayedInitialize);
    delayedInitializeTimer->start();
}

/*!
    \internal
*/
void PluginManagerPrivate::shutdown()
{
    stopAll();
    if (!asynchronousPlugins.isEmpty()) {
        shutdownEventLoop = new QEventLoop;
        shutdownEventLoop->exec();
    }
    deleteAll();
    if (!allObjects.isEmpty()) {
        qDebug() << "There are" << allObjects.size() << "objects left in the plugin manager pool.";
        // Intentionally split debug info here, since in case the list contains
        // already deleted object we get at least the info about the number of objects;
        qDebug() << "The following objects left in the plugin manager pool:" << allObjects;
    }
}

/*!
    \internal
*/
void PluginManagerPrivate::asyncShutdownFinished()
{
    IPlugin *plugin = qobject_cast<IPlugin *>(sender());
    Q_ASSERT(plugin);
    asynchronousPlugins.removeAll(plugin->pluginSpec());
    if (asynchronousPlugins.isEmpty())
        shutdownEventLoop->exit();
}

/*!
    \internal
*/
QList<PluginSpec *> PluginManagerPrivate::loadQueue()
{
    QList<PluginSpec *> queue;
    foreach (PluginSpec *spec, pluginSpecs) {
        QList<PluginSpec *> circularityCheckQueue;
        loadQueue(spec, queue, circularityCheckQueue);
    }
    return queue;
}

/*!
    \internal
*/
bool PluginManagerPrivate::loadQueue(PluginSpec *spec, QList<PluginSpec *> &queue,
        QList<PluginSpec *> &circularityCheckQueue)
{
    if (queue.contains(spec))
        return true;
    // check for circular dependencies
    if (circularityCheckQueue.contains(spec)) {
        spec->d->hasError = true;
        spec->d->errorString = PluginManager::tr("Circular dependency detected:");
        spec->d->errorString += QLatin1Char('\n');
        int index = circularityCheckQueue.indexOf(spec);
        for (int i = index; i < circularityCheckQueue.size(); ++i) {
            spec->d->errorString.append(PluginManager::tr("%1(%2) depends on")
                .arg(circularityCheckQueue.at(i)->name()).arg(circularityCheckQueue.at(i)->version()));
            spec->d->errorString += QLatin1Char('\n');
        }
        spec->d->errorString.append(PluginManager::tr("%1(%2)").arg(spec->name()).arg(spec->version()));
        return false;
    }
    circularityCheckQueue.append(spec);
    // check if we have the dependencies
    if (spec->state() == PluginSpec::Invalid || spec->state() == PluginSpec::Read) {
        queue.append(spec);
        return false;
    }

    // add dependencies
    QHashIterator<PluginDependency, PluginSpec *> it(spec->dependencySpecs());
    while (it.hasNext()) {
        it.next();
        // Skip test dependencies since they are not real dependencies but just force-loaded
        // plugins when running tests
        if (it.key().type == PluginDependency::Test)
            continue;
        PluginSpec *depSpec = it.value();
        if (!loadQueue(depSpec, queue, circularityCheckQueue)) {
            spec->d->hasError = true;
            spec->d->errorString =
                PluginManager::tr("Cannot load plugin because dependency failed to load: %1(%2)\nReason: %3")
                    .arg(depSpec->name()).arg(depSpec->version()).arg(depSpec->errorString());
            return false;
        }
    }
    // add self
    queue.append(spec);
    return true;
}

/*!
    \internal
*/
void PluginManagerPrivate::loadPlugin(PluginSpec *spec, PluginSpec::State destState)
{
    if (spec->hasError() || spec->state() != destState-1)
        return;

    // don't load disabled plugins.
    if (!spec->isEffectivelyEnabled() && destState == PluginSpec::Loaded)
        return;

    switch (destState) {
    case PluginSpec::Running:
        profilingReport(">initializeExtensions", spec);
        spec->d->initializeExtensions();
        profilingReport("<initializeExtensions", spec);
        return;
    case PluginSpec::Deleted:
        profilingReport(">delete", spec);
        spec->d->kill();
        profilingReport("<delete", spec);
        return;
    default:
        break;
    }
    // check if dependencies have loaded without error
    QHashIterator<PluginDependency, PluginSpec *> it(spec->dependencySpecs());
    while (it.hasNext()) {
        it.next();
        if (it.key().type != PluginDependency::Required)
            continue;
        PluginSpec *depSpec = it.value();
        if (depSpec->state() != destState) {
            spec->d->hasError = true;
            spec->d->errorString =
                PluginManager::tr("Cannot load plugin because dependency failed to load: %1(%2)\nReason: %3")
                    .arg(depSpec->name()).arg(depSpec->version()).arg(depSpec->errorString());
            return;
        }
    }
    switch (destState) {
    case PluginSpec::Loaded:
        profilingReport(">loadLibrary", spec);
        spec->d->loadLibrary();
        profilingReport("<loadLibrary", spec);
        break;
    case PluginSpec::Initialized:
        profilingReport(">initializePlugin", spec);
        spec->d->initializePlugin();
        profilingReport("<initializePlugin", spec);
        break;
    case PluginSpec::Stopped:
        profilingReport(">stop", spec);
        if (spec->d->stop() == IPlugin::AsynchronousShutdown) {
            asynchronousPlugins << spec;
            connect(spec->plugin(), &IPlugin::asynchronousShutdownFinished,
                    this, &PluginManagerPrivate::asyncShutdownFinished);
        }
        profilingReport("<stop", spec);
        break;
    default:
        break;
    }
}

/*!
    \internal
*/
void PluginManagerPrivate::setPluginPaths(const QStringList &paths)
{
    qCDebug(pluginLog) << "Plugin search paths:" << paths;
    qCDebug(pluginLog) << "Required IID:" << pluginIID;
    pluginPaths = paths;
    readSettings();
    readPluginPaths();
}

static QStringList pluginFiles(const QStringList &pluginPaths)
{
    QStringList pluginFiles;
    QStringList searchPaths = pluginPaths;
    while (!searchPaths.isEmpty()) {
        const QDir dir(searchPaths.takeFirst());
        const QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
        const QStringList absoluteFilePaths = Utils::transform(files, &QFileInfo::absoluteFilePath);
        pluginFiles += Utils::filtered(absoluteFilePaths, [](const QString &path) { return QLibrary::isLibrary(path); });
        const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        searchPaths += Utils::transform(dirs, &QFileInfo::absoluteFilePath);
    }
    return pluginFiles;
}

/*!
    \internal
*/
void PluginManagerPrivate::readPluginPaths()
{
    qDeleteAll(pluginSpecs);
    pluginSpecs.clear();
    pluginCategories.clear();

    // default
    pluginCategories.insert(QString(), QList<PluginSpec *>());

    foreach (const QString &pluginFile, pluginFiles(pluginPaths)) {
        PluginSpec *spec = new PluginSpec;
        if (!spec->d->read(pluginFile)) { // not a Qt Creator plugin
            delete spec;
            continue;
        }

        // defaultDisabledPlugins and defaultEnabledPlugins from install settings
        // is used to override the defaults read from the plugin spec
        if (spec->isEnabledByDefault() && defaultDisabledPlugins.contains(spec->name())) {
            spec->d->setEnabledByDefault(false);
            spec->d->setEnabledBySettings(false);
        } else if (!spec->isEnabledByDefault() && defaultEnabledPlugins.contains(spec->name())) {
            spec->d->setEnabledByDefault(true);
            spec->d->setEnabledBySettings(true);
        }
        if (!spec->isEnabledByDefault() && forceEnabledPlugins.contains(spec->name()))
            spec->d->setEnabledBySettings(true);
        if (spec->isEnabledByDefault() && disabledPlugins.contains(spec->name()))
            spec->d->setEnabledBySettings(false);

        pluginCategories[spec->category()].append(spec);
        pluginSpecs.append(spec);
    }
    resolveDependencies();
    // ensure deterministic plugin load order by sorting
    Utils::sort(pluginSpecs, &PluginSpec::name);
    emit q->pluginsChanged();
}

void PluginManagerPrivate::resolveDependencies()
{
    foreach (PluginSpec *spec, pluginSpecs) {
        spec->d->enabledIndirectly = false; // reset, is recalculated below
        spec->d->resolveDependencies(pluginSpecs);
    }

    Utils::reverseForeach(loadQueue(), [](PluginSpec *spec) {
        spec->d->enableDependenciesIndirectly();
    });
}

void PluginManagerPrivate::enableOnlyTestedSpecs()
{
    if (testSpecs.isEmpty())
        return;

    QList<PluginSpec *> specsForTests;
    foreach (const TestSpec &testSpec, testSpecs) {
        QList<PluginSpec *> circularityCheckQueue;
        loadQueue(testSpec.pluginSpec, specsForTests, circularityCheckQueue);
        // add plugins that must be force loaded when running tests for the plugin
        // (aka "test dependencies")
        QHashIterator<PluginDependency, PluginSpec *> it(testSpec.pluginSpec->dependencySpecs());
        while (it.hasNext()) {
            it.next();
            if (it.key().type != PluginDependency::Test)
                continue;
            PluginSpec *depSpec = it.value();
            circularityCheckQueue.clear();
            loadQueue(depSpec, specsForTests, circularityCheckQueue);
        }
    }
    foreach (PluginSpec *spec, pluginSpecs)
        spec->d->setForceDisabled(true);
    foreach (PluginSpec *spec, specsForTests) {
        spec->d->setForceDisabled(false);
        spec->d->setForceEnabled(true);
    }
}

// Look in argument descriptions of the specs for the option.
PluginSpec *PluginManagerPrivate::pluginForOption(const QString &option, bool *requiresArgument) const
{
    // Look in the plugins for an option
    *requiresArgument = false;
    foreach (PluginSpec *spec, pluginSpecs) {
        PluginArgumentDescription match = Utils::findOrDefault(spec->argumentDescriptions(),
                                                               [option](PluginArgumentDescription pad) {
                                                                   return pad.name == option;
                                                               });
        if (!match.name.isEmpty()) {
            *requiresArgument = !match.parameter.isEmpty();
            return spec;
        }
    }
    return 0;
}

PluginSpec *PluginManagerPrivate::pluginByName(const QString &name) const
{
    return Utils::findOrDefault(pluginSpecs, [name](PluginSpec *spec) { return spec->name() == name; });
}

void PluginManagerPrivate::initProfiling()
{
    if (m_profileTimer.isNull()) {
        m_profileTimer.reset(new QTime);
        m_profileTimer->start();
        m_profileElapsedMS = 0;
        qDebug("Profiling started");
    } else {
        m_profilingVerbosity++;
    }
}

void PluginManagerPrivate::profilingReport(const char *what, const PluginSpec *spec /* = 0 */)
{
    if (!m_profileTimer.isNull()) {
        const int absoluteElapsedMS = m_profileTimer->elapsed();
        const int elapsedMS = absoluteElapsedMS - m_profileElapsedMS;
        m_profileElapsedMS = absoluteElapsedMS;
        if (spec)
            m_profileTotal[spec] += elapsedMS;
        if (spec)
            qDebug("%-22s %-22s %8dms (%8dms)", what, qPrintable(spec->name()), absoluteElapsedMS, elapsedMS);
        else
            qDebug("%-45s %8dms (%8dms)", what, absoluteElapsedMS, elapsedMS);
    }
}

void PluginManagerPrivate::profilingSummary() const
{
    if (!m_profileTimer.isNull()) {
        QMultiMap<int, const PluginSpec *> sorter;
        int total = 0;

        auto totalEnd = m_profileTotal.constEnd();
        for (auto it = m_profileTotal.constBegin(); it != totalEnd; ++it) {
            sorter.insert(it.value(), it.key());
            total += it.value();
        }

        auto sorterEnd = sorter.constEnd();
        for (auto it = sorter.constBegin(); it != sorterEnd; ++it)
            qDebug("%-22s %8dms   ( %5.2f%% )", qPrintable(it.value()->name()),
                it.key(), 100.0 * it.key() / total);
         qDebug("Total: %8dms", total);
    }
}

static inline QString getPlatformName()
{
#if 0
    if (HostOsInfo::isMacHost()) {
        if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_0) {
            QString result = QLatin1String("OS X");
            result += QLatin1String(" 10.") + QString::number(QSysInfo::MacintoshVersion - QSysInfo::MV_10_0);
            return result;
        } else {
            return QLatin1String("Mac OS");
        }
    } else if (HostOsInfo::isAnyUnixHost()) {
        QString base = QLatin1String(HostOsInfo::isLinuxHost() ? "Linux" : "Unix");
        QFile osReleaseFile(QLatin1String("/etc/os-release")); // Newer Linuxes
        if (osReleaseFile.open(QIODevice::ReadOnly)) {
            QString name;
            QString version;
            forever {
                const QByteArray line = osReleaseFile.readLine();
                if (line.isEmpty())
                    break;
                if (line.startsWith("NAME=\""))
                    name = QString::fromLatin1(line.mid(6, line.size() - 8)).trimmed();
                if (line.startsWith("VERSION_ID=\""))
                    version = QString::fromLatin1(line.mid(12, line.size() - 14)).trimmed();
            }
            if (!name.isEmpty()) {
                if (!version.isEmpty())
                    name += QLatin1Char(' ') + version;
                return base + QLatin1String(" (") + name + QLatin1Char(')');
            }
        }
        return base;
    } else if (HostOsInfo::isWindowsHost()) {
        QString result = QLatin1String("Windows");
        switch (QSysInfo::WindowsVersion) {
        case QSysInfo::WV_XP:
            result += QLatin1String(" XP");
            break;
        case QSysInfo::WV_2003:
            result += QLatin1String(" 2003");
            break;
        case QSysInfo::WV_VISTA:
            result += QLatin1String(" Vista");
            break;
        case QSysInfo::WV_WINDOWS7:
            result += QLatin1String(" 7");
            break;
        case QSysInfo::WV_WINDOWS8:
            result += QLatin1String(" 8");
            break;
        case QSysInfo::WV_WINDOWS8_1:
            result += QLatin1String(" 8.1");
            break;
        default:
            break;
        }
        if (QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS10)
            result += QLatin1String(" 10");
        return result;
    }
#endif
    return QLatin1String("Unknown");
}

QString PluginManager::platformName()
{
    static const QString result = getPlatformName();
    return result;
}

bool PluginManager::isInitializationDone()
{
    return d->m_isInitializationDone;
}

/*!
    Retrieves one object with \a name from the object pool.
    \sa addObject()
*/

QObject *PluginManager::getObjectByName(const QString &name)
{
    QReadLocker lock(&d->m_lock);
    return Utils::findOrDefault(allObjects(), [&name](const QObject *obj) {
        return obj->objectName() == name;
    });
}

/*!
    Retrieves one object inheriting a class with \a className from the object
    pool.
    \sa addObject()
*/

QObject *PluginManager::getObjectByClassName(const QString &className)
{
    const QByteArray ba = className.toUtf8();
    QReadLocker lock(&d->m_lock);
    return Utils::findOrDefault(allObjects(), [&ba](const QObject *obj) {
        return obj->inherits(ba.constData());
    });
}
