#include "iomenuiconfactory.h"
#include <QPushButton>
#include <QLabel>

using namespace Core;


ioMenuIconFactory::ioMenuIconFactory()
{

}

MenuIconView ioMenuIconFactory::create()
{
    auto *p = new QPushButton;
    auto *l = new QLabel;
    MenuIconView n(p, l);
    return n;
}

