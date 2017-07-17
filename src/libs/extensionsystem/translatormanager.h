#ifndef TRANSLATORMANAGER_H
#define TRANSLATORMANAGER_H

#include "extensionsystem_global.h"
#include <QObject>

namespace ExtensionSystem
{
namespace Internal { class TranslatorManagerPrivate; }
class EXTENSIONSYSTEM_EXPORT TranslatorManager : public QObject
{
    Q_OBJECT
public:
    explicit TranslatorManager(QObject *parent = nullptr);
    static TranslatorManager *instance();
    static void load();
    static bool changeLanguage(const QString &name);
signals:
    void languageChanged(const QString &name);
public slots:


    friend class Internal::TranslatorManagerPrivate;
};

} // namespace ExtensionSystem

#endif // TRANSLATORMANAGER_H
