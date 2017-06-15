#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "imenuiconfactory.h"
#include <extensionsystem/pluginmanager.h>

#include <QPainter>
#include <QListWidgetItem>

using namespace ExtensionSystem;

namespace Core {
namespace Internal {

mainmenu::mainmenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainmenu)
{
    ui->setupUi(this);





}

mainmenu::~mainmenu()
{
    delete ui;
}

void mainmenu::extensionsInitialized()
{
    m_listIMenuIcons = PluginManager::getObjects<IMenuIconFactory>();
    for (auto* factory : m_listIMenuIcons) {
        factory->create();
    }
}

} // namespace Internal
} // namespace Core

