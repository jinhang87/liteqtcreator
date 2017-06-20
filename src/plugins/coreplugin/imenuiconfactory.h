#ifndef IMENUICONFACTORY_H
#define IMENUICONFACTORY_H

#include "core_global.h"
#include <QObject>

#include<QPushButton>
#include<QLabel>

namespace Core {

struct MenuIconView
{
    MenuIconView(QPushButton *b = 0,QLabel *l = 0 ) : m_button(b),m_label(l) {}
public:
    void show(){
        if(m_button){m_button->show();}
        if(m_label){m_label->show();}
    }
    QPushButton *m_button;
    QLabel *m_label;
};

class CORE_EXPORT IMenuIconFactory : public QObject
{
    Q_OBJECT
public:
    explicit IMenuIconFactory();
    virtual MenuIconView create(QWidget *parent = 0) = 0;
    virtual void retranslate(MenuIconView &view) = 0;
signals:

public slots:
};

}
#endif // IMENUICONFACTORY_H
