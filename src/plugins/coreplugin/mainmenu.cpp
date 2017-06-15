#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "imenuiconfactory.h"
#include <extensionsystem/pluginmanager.h>

#include <QPainter>
#include <QListWidgetItem>
#include <QDebug>

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
    delete ui;
}

void mainmenu::extensionsInitialized()
{
    qDebug() << "mainmenu::extensionsInitialized";
    m_listIMenuIcons = PluginManager::getObjects<IMenuIconFactory>();
    for (auto* factory : m_listIMenuIcons) {
        auto view = factory->create(this);
        qDebug() << "mainmenu::factory->create";
        view.m_button->setGeometry(10,10,200,200);
        view.show();

    }

}

} // namespace Internal
} // namespace Core

