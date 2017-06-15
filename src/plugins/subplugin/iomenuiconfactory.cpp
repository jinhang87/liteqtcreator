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
    auto *l = new QLabel(parent);
    MenuIconView n(p, l);
    return n;
}

