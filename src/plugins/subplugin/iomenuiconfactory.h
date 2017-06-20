#ifndef IOMENUICONFACTORY_H
#define IOMENUICONFACTORY_H

#include <coreplugin/imenuiconfactory.h>
#include <QObject>

class ioMenuIconFactory : public Core::IMenuIconFactory
{
    Q_OBJECT
public:
    explicit ioMenuIconFactory();
    Core::MenuIconView create(QWidget *parent = 0) override;
    void retranslate(Core::MenuIconView &view) override;
signals:

public slots:
};

#endif // IOMENUICONFACTORY_H
