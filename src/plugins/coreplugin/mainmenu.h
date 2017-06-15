#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>

namespace Ui {
class mainmenu;
}

namespace Core {
class IMenuIconFactory;

namespace Internal {

class mainmenu : public QWidget
{
    Q_OBJECT

public:
    explicit mainmenu(QWidget *parent = 0);
    ~mainmenu();
    void extensionsInitialized();

private:
    Ui::mainmenu *ui;
    QList<IMenuIconFactory*> m_listIMenuIcons;
};

} // namespace Internal
} // namespace Core

#endif // MAINMENU_H
