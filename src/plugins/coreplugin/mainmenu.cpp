#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "imenuiconfactory.h"
#include <extensionsystem/pluginmanager.h>

using namespace ExtensionSystem;

namespace Core {
namespace Internal {

mainmenu::mainmenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainmenu)
{
    qDebug() << "mainmenu::mainmenu";
    ui->setupUi(this);
}

mainmenu::~mainmenu()
{
    qDebug() << "mainmenu::~mainmenu";
    delete ui;
}

void mainmenu::extensionsInitialized()
{
    qDebug() << "mainmenu::extensionsInitialized";
    m_listIMenuIcons = PluginManager::getObjects<IMenuIconFactory>();
    for (auto* factory : m_listIMenuIcons) {
        auto view = factory->create(this);
        view.m_button->setGeometry(10,10,200,200);
        view.show();
        m_listIconView << view;
    }

}

void mainmenu::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        this->retranslate();
    }
}

void mainmenu::retranslate()
{
    m_listIMenuIcons = PluginManager::getObjects<IMenuIconFactory>();
    int i = 0;
    for (i = 0; i < m_listIMenuIcons.size(); ++i) {
        m_listIMenuIcons.at(i)->retranslate(m_listIconView[i]);
    }
}

} // namespace Internal
} // namespace Core

