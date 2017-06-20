#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>

namespace Ui {
class mainmenu;
}

namespace Core {
class IMenuIconFactory;
struct MenuIconView;

namespace Internal {

class mainmenu : public QWidget
{
    Q_OBJECT

public:
    explicit mainmenu(QWidget *parent = 0);
    ~mainmenu();
    void extensionsInitialized();
    void changeEvent(QEvent *event) override;

private:
    void retranslate();

    Ui::mainmenu *ui;
    QList<IMenuIconFactory*> m_listIMenuIcons;
    QList<MenuIconView> m_listIconView;
};

} // namespace Internal
} // namespace Core

#endif // MAINMENU_H
