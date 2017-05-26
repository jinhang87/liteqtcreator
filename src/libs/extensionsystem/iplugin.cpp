

#include "iplugin.h"
#include "iplugin_p.h"
#include "pluginmanager.h"
#include "pluginspec.h"

using namespace ExtensionSystem;

/*!
    \fn IPlugin::IPlugin()
    \internal
*/
IPlugin::IPlugin()
    : d(new Internal::IPluginPrivate())
{
}

/*!
    \fn IPlugin::~IPlugin()
    \internal
*/
IPlugin::~IPlugin()
{
    foreach (QObject *obj, d->addedObjectsInReverseOrder)
        PluginManager::removeObject(obj);
    qDeleteAll(d->addedObjectsInReverseOrder);
    d->addedObjectsInReverseOrder.clear();
    delete d;
    d = 0;
}

/*!
    \fn QList<QObject *> IPlugin::createTestObjects() const

    Returns objects that are meant to be passed on to QTest::qExec().

    This function will be called if the user starts \QC with '-test PluginName' or '-test all'.

    The ownership of returned objects is transferred to caller.
*/
QList<QObject *> IPlugin::createTestObjects() const
{
    return QList<QObject *>();
}

/*!
    \fn PluginSpec *IPlugin::pluginSpec() const
    Returns the PluginSpec corresponding to this plugin.
    This is not available in the constructor.
*/
PluginSpec *IPlugin::pluginSpec() const
{
    return d->pluginSpec;
}

/*!
    \fn void IPlugin::addObject(QObject *obj)
    Convenience function that registers \a obj in the plugin manager's
    plugin pool by just calling PluginManager::addObject().
*/
void IPlugin::addObject(QObject *obj)
{
    PluginManager::addObject(obj);
}

/*!
    \fn void IPlugin::addAutoReleasedObject(QObject *obj)
    Convenience function for registering \a obj in the plugin manager's
    plugin pool. Usually, registered objects must be removed from
    the object pool and deleted by hand.
    Objects added to the pool via addAutoReleasedObject are automatically
    removed and deleted in reverse order of registration when
    the IPlugin instance is destroyed.
    \sa PluginManager::addObject()
*/
void IPlugin::addAutoReleasedObject(QObject *obj)
{
    d->addedObjectsInReverseOrder.prepend(obj);
    PluginManager::addObject(obj);
}

/*!
    \fn void IPlugin::removeObject(QObject *obj)
    Convenience function that unregisters \a obj from the plugin manager's
    plugin pool by just calling PluginManager::removeObject().
*/
void IPlugin::removeObject(QObject *obj)
{
    PluginManager::removeObject(obj);
}

