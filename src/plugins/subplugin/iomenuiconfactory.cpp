#include "iomenuiconfactory.h"
#include <QPushButton>
#include <QLabel>

using namespace Core;


ioMenuIconFactory::ioMenuIconFactory()
{

}

MenuIconView ioMenuIconFactory::create(QWidget *parent)
{
    auto *p = new QPushButton(parent);
    p->setText(tr("button"));
    auto *l = new QLabel(parent);
    l->setText(tr("label"));
    MenuIconView n(p, l);
    return n;
}

