#include "iomenuiconfactory.h"
#include <QPushButton>
#include <QLabel>
#include "extensionsystem/translatormanager.h"

using namespace Core;
using namespace ExtensionSystem;

ioMenuIconFactory::ioMenuIconFactory()
{

}

MenuIconView ioMenuIconFactory::create(QWidget *parent)
{
    auto *p = new QPushButton(parent);
    connect(p, &QPushButton::clicked, this, [](){
        static int i = 0;
        if(i++%2 == 0)
            TranslatorManager::changeLanguage("zh_CN");
        else
            TranslatorManager::changeLanguage("en_US");
    });
    auto *l = new QLabel(parent);
    MenuIconView n(p, l);
    retranslate(n);
    return n;
}

void ioMenuIconFactory::retranslate(MenuIconView &view)
{
    view.m_button->setText(tr("button"));
    view.m_label->setText(tr("label"));
}

