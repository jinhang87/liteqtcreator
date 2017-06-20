#ifndef TANSLATORMANAGER_P_H
#define TANSLATORMANAGER_P_H

#include "extensionsystem_global.h"
#include <QObject>
#include <QHash>
#include <QTranslator>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace ExtensionSystem {

class TranslatorManager;

namespace Internal {

class EXTENSIONSYSTEM_EXPORT TranslatorManagerPrivate : public QObject
{
    Q_OBJECT
public:
    TranslatorManagerPrivate(TranslatorManager *translatorManager);
    virtual ~TranslatorManagerPrivate();
    void load();
    bool changeLanguage(const QString &name);
    const QString getTrPath();
    const QString getQtTrPath();
private:
    TranslatorManager *q;
    //QTranslator* m_translaors;
    //QTranslator* m_qtTranslaors;
    QHash<QString, QTranslator*> m_translators;
    QHash<QString, QTranslator*> m_qtTranslaors;
    QString m_lastLanguage;
};

} //namespace Internal

} //namespace ExtensionSystem


#endif // TANSLATORMANAGER_P_H
