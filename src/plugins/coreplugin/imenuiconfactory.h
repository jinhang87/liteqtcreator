#ifndef IMENUICONFACTORY_H
#define IMENUICONFACTORY_H

#include <QObject>

class QPushButton;
class QLabel;

namespace Core {

struct MenuIconView
{
    MenuIconView(QPushButton *b = 0,QLabel *l = 0 ) : m_button(b),m_label(l) {}
    QPushButton *m_button;
    QLabel *m_label;
};

class IMenuIconFactory : public QObject
{
    Q_OBJECT
public:
    explicit IMenuIconFactory();
    virtual MenuIconView create() = 0;

signals:

public slots:
};

}
#endif // IMENUICONFACTORY_H
