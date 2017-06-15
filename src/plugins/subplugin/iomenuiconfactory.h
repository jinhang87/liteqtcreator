#ifndef IOMENUICONFACTORY_H
#define IOMENUICONFACTORY_H

#include <coreplugin/imenuiconfactory.h>
#include <QObject>

class ioMenuIconFactory : public Core::IMenuIconFactory
{
    Q_OBJECT
public:
    explicit ioMenuIconFactory();

signals:

public slots:
};

#endif // IOMENUICONFACTORY_H
